#include "Audio.h"
#include <cassert>
#include <algorithm>
#include "Logger.h"

using namespace Logger;

// チャンクヘッダ
struct ChunkHeader {
	char id[4];   // チャンクID
	int32_t size; // チャンクサイズ
};

// RIFFヘッダチャンク
struct RiffHeader {
	ChunkHeader chunk; // "RIFF"
	char tpye[4];      // "WAVE"
};

// FMTチャンク
struct FormatChunk {
	ChunkHeader chunk; // "fmt"
	WAVEFORMATEX fmt;  // 波形フォーマット
};

// 同時に再生できる最大数
const uint32_t Audio::maxSourceVoiceCount = 64;

Audio* Audio::instance = nullptr;


Audio* Audio::GetInstance() {
	if (instance == nullptr) {
		instance = new Audio;
	}
	return instance;
}

void Audio::Finalize() {
	// Stop / Destroy voices
	for (AudioList list : audioList)
	{
		if (list.sourceVoice) {
			list.sourceVoice->Stop();
			list.sourceVoice->DestroyVoice();
		}
	}
	audioList.clear();
	// 音声データのコンテナをクリア
	soundMap.clear();
	mp3AudioData.clear();

	// Media FoundationとCOMの終了
	if (mfStarted) {
		HRESULT hr = MFShutdown();
		if (FAILED(hr)) {
			char buf[128];
			sprintf_s(buf, "MFShutdown failed: 0x%08X\n", static_cast<unsigned>(hr));
			Log(buf);
		}
		mfStarted = false;
	}
	if (comInitialized) {
		CoUninitialize();
		comInitialized = false;
	}

	delete instance;
	instance = nullptr;
}

void Audio::Initialize() {
	
	xAudio2.Reset();
	HRESULT hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr)) {
		char buf[128];
		sprintf_s(buf, "XAudio2Create failed: 0x%08X\n", static_cast<unsigned>(hr));
		Log(buf);
		assert(false);
		return;
	}

	hr = xAudio2->CreateMasteringVoice(&masterVoice);
	if (FAILED(hr)) {
		char buf[128];
		sprintf_s(buf, "CreateMasteringVoice failed: 0x%08X\n", static_cast<unsigned>(hr));
		Log(buf);
		xAudio2.Reset();
		assert(false);
		return;
	}

	// mp3読み込みのためのMedia Foundationの初期化
	HRESULT co = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (SUCCEEDED(co)) {
		comInitialized = true;
	} else if (co == RPC_E_CHANGED_MODE) {
		// 既に別のスレッドモデルで初期化済み（許容） — 継続するがフラグは立てない
		Log("CoInitializeEx returned RPC_E_CHANGED_MODE\n");
	} else {
		char buf[128];
		sprintf_s(buf, "CoInitializeEx failed: 0x%08X\n", static_cast<unsigned>(co));
		Log(buf);
		// 続けるが Media Foundation が失敗する可能性があるため注意
	}

	HRESULT mfr = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	if (SUCCEEDED(mfr)) {
		mfStarted = true;
	} else {
		char buf[128];
		sprintf_s(buf, "MFStartup failed: 0x%08X\n", static_cast<unsigned>(mfr));
		Log(buf);
		// 失敗してもアプリの他機能は動く可能性があるので即終了はしない
	}
}

bool Audio::LoadWave(const std::string filePath, const std::string soundName, const float volume) {
	// 登録する名前が重複していたらfalseでreturn
	if (soundMap.contains(soundName))
	{
		return false;
		Log("this name already registered/n");
	}
	// 登録する音声が重複していたらfalseでreturn
	for (auto it = soundMap.begin(); it != soundMap.end(); ++it)
	{
		if (it->second.filePath == filePath)
		{
			Log("this file loaded\n");
			return false;
		}
	}


	/// ファイルオープン

	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(filePath, std::ios_base::binary);
	if (!file.is_open())
	{
		// ファイルオープン失敗を検出する
#ifdef _DEBUG
		assert(0);
#endif // _DEBUG

		Log("Cant open file\n");
		return false;
	}

	/// .wavデータ読み込み

	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}

	// タイプがWAVEかチェック
	if (strncmp(riff.tpye, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	// JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	int time = data.size / format.fmt.nAvgBytesPerSec;
	if (time <= 0)
	{
		time = 1;
	}

	// Waveファイルを閉じる
	file.close();

	float vol = std::clamp(volume, 0.0f, 1.0f);

	// returnする為の音声データ
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.filePath = filePath;
	soundData.playTime = static_cast<float>(time);
	soundData.volume = vol;

	soundMap[soundName] = soundData;

	return true;
}

// 音声再生
void Audio::Play(const std::string soundName, const bool loop) {

	if (!soundMap.contains(soundName))
	{
		Log("this name is not registered\n");
		return;
	}

	HRESULT result;

	// 波形フォーマットをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundMap[soundName].wfex);
	assert(SUCCEEDED(result));

	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.AudioBytes = soundMap[soundName].bufferSize;
	buf.pAudioData = soundMap[soundName].pBuffer;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loop)
	{
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->SetVolume(soundMap[soundName].volume);
	result = pSourceVoice->Start();
	AudioList list = { pSourceVoice, soundMap[soundName], buf, frameTime };
	audioList.push_back(list);
	// 指定したsourceVoiceよりも多くpush_backしたらassert
	assert(audioList.size() < maxSourceVoiceCount);
}

bool Audio::LoadMP3(const std::string filePath, const std::string soundName, const float volume) {
	// 登録する名前が重複していたらfalseでreturn
	if (soundMap.contains(soundName))
	{
		return false;
		Log("this name already registered\n");
	}
	// 登録する音声が重複していたらfalseでreturn
	for (auto it = soundMap.begin(); it != soundMap.end(); ++it)
	{
		if (it->second.filePath == filePath)
		{
			Log("this file loaded\n");
			return false;
		}
	}
	// Media Foundationオブジェクト
	Microsoft::WRL::ComPtr<IMFSourceReader> sourceReader;
	Microsoft::WRL::ComPtr<IMFMediaType> mediaType;

	// ファイルパスをワイド文字列に変換
	wchar_t wFilePath[MAX_PATH];
	size_t convertedChars = 0;
	mbstowcs_s(&convertedChars, wFilePath, filePath.c_str(), MAX_PATH);

	// ソースリーダーの作成
	HRESULT hr = MFCreateSourceReaderFromURL(wFilePath, nullptr, sourceReader.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// メディアタイプの取得
	hr = sourceReader->GetNativeMediaType(
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		0,
		mediaType.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// PCMフォーマットに変換
	hr = MFCreateMediaType(mediaType.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	hr = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	if (FAILED(hr)) {
		return false;
	}

	hr = mediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	if (FAILED(hr)) {
		return false;
	}

	// メディアタイプの設定
	hr = sourceReader->SetCurrentMediaType(
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		nullptr,
		mediaType.Get());
	if (FAILED(hr)) {
		return false;
	}

	// 変換後のメディアタイプを取得
	hr = sourceReader->GetCurrentMediaType(
		MF_SOURCE_READER_FIRST_AUDIO_STREAM,
		mediaType.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	// WAVE情報を取得
	UINT32 formatSize = 0;
	WAVEFORMATEX* pWaveFormat = nullptr;
	hr = MFCreateWaveFormatExFromMFMediaType(
		mediaType.Get(),
		&pWaveFormat,
		&formatSize);
	if (FAILED(hr)) {
		return false;
	}

	// WAVEをコピー
	memcpy(&mp3waveFormat, pWaveFormat, sizeof(WAVEFORMATEX));

	SoundData soundData;

	while (true) {
		// サンプルの読み込み
		Microsoft::WRL::ComPtr<IMFSample> sample;
		DWORD streamFlags = 0;

		hr = sourceReader->ReadSample(
			MF_SOURCE_READER_FIRST_AUDIO_STREAM,
			0,
			nullptr,
			&streamFlags,
			nullptr,
			sample.GetAddressOf());

		if (FAILED(hr) || (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)) {
			break;
		}

		if (sample == nullptr) {
			continue;
		}

		// サンプルからメディアバッファを取得
		Microsoft::WRL::ComPtr<IMFMediaBuffer> mediaBuffer;
		hr = sample->ConvertToContiguousBuffer(mediaBuffer.GetAddressOf());
		if (FAILED(hr)) {
			continue;
		}

		// メディアバッファからデータを取得
		BYTE* audioBuffer = nullptr;
		DWORD bufferSize = 0;

		hr = mediaBuffer->Lock(&audioBuffer, nullptr, &bufferSize);
		if (FAILED(hr)) {
			continue;
		}


		// データをコピー
		size_t offset = mp3AudioData[soundName].size();
		mp3AudioData[soundName].resize(offset + bufferSize);
		memcpy(mp3AudioData[soundName].data() + offset, audioBuffer, bufferSize);

		mediaBuffer->Unlock();
	}

	// 再生時間を計算する
	int time = static_cast<int>(mp3AudioData.size() / pWaveFormat->nAvgBytesPerSec);
	if (time <= 0)
	{
		time = 1;
	}
	CoTaskMemFree(pWaveFormat);

	float vol = std::clamp(volume, 0.0f, 1.0f);

	soundData.wfex = mp3waveFormat;
	soundData.bufferSize = static_cast<UINT32>(mp3AudioData[soundName].size());
	soundData.pBuffer = mp3AudioData[soundName].data();
	soundData.filePath = filePath;
	soundData.volume = vol;
	soundData.playTime = static_cast<float>(time);

	soundMap[soundName] = soundData;

	Log("Audio load\n");

	mp3waveFormat = {};
	return true;
}

void Audio::SetVolume(const std::string soundName, const float volume) {

	float vol = std::clamp(volume, 0.0f, 1.0f);
	soundMap[soundName].volume = vol;
	for (AudioList list : audioList)
	{
		if (list.soundData.filePath == soundMap[soundName].filePath)
		{
			list.sourceVoice->SetVolume(soundMap[soundName].volume);
			return;
		}
	}
}

void Audio::SetMasterVolume(const float volume) {

	float vol = std::clamp(volume, 0.0f, 1.0f);
	if (!masterVoice) {
		Log("SetMasterVolume called but masterVoice is null\n");
		return;
	}
	masterVoice->SetVolume(vol);
}

// 全ての音声停止
void Audio::StopAll() {
	// listに登録されている全てを音声停止してlistをclearする
	for (AudioList list : audioList)
	{
		list.sourceVoice->Stop();
		list.sourceVoice->DestroyVoice();
	}
	audioList.clear();
}

// 音声停止
void Audio::Stop(const std::string soundName) {
	// listに登録されている中から指定されたfilenameに一致するもの全てを音声停止して一致するものをlistからremoveする
	uint32_t index = 0;
	uint32_t eraseList[maxSourceVoiceCount] = { 0 };
	uint32_t eraseNum = 0;
	for (AudioList list : audioList)
	{
		if (list.soundData.filePath == soundMap[soundName].filePath)
		{
			list.sourceVoice->Stop();
			list.sourceVoice->DestroyVoice();
			eraseList[eraseNum] = index;
			eraseNum++;
		}
		index++;
	}
	for (uint32_t i = 0; i < eraseNum; i++)
	{
		audioList.erase(audioList.begin() + eraseList[i]);
		eraseList[i + 1] -= i + 1;
	}
}

void Audio::Pause(const std::string soundName) {
	for (AudioList list : audioList)
	{
		if (list.soundData.filePath == soundMap[soundName].filePath)
		{
			list.sourceVoice->Stop();
		}
	}
}

void Audio::Resume(const std::string soundName) {
	for (AudioList list : audioList)
	{
		if (list.soundData.filePath == soundMap[soundName].filePath)
		{
			list.sourceVoice->Start();
		}
	}
}

void Audio::Update() {
	// audioListが0なら早期return
	if (audioList.size() == 0) {
		frameTime = 0;
		return;
	}
	uint32_t index = 0;
	for (AudioList list : audioList)
	{
		if (frameTime >= list.soundData.playTime * 60 + list.startFrameTime && list.buf.LoopCount != XAUDIO2_LOOP_INFINITE)
		{
			// 再生時間ごとに削除 
			// listのbufferがループになっていないものを対象にする
			list.sourceVoice->Stop();
			list.sourceVoice->DestroyVoice();
			audioList.erase(audioList.begin() + index);
			break;
		}
		index++;
	}
	frameTime++;
}

// 音声データ解放
void Audio::SoundUnload(const std::string soundName) {
	// バッファのメモリを解放

	soundMap.erase(soundName);
	mp3AudioData.erase(soundName);
	Log("sound unloaded\n");
}

void Audio::Unload(SoundData* soundData) {
	// バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}
