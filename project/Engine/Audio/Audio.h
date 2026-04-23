#pragma once

#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <vector>
#include <map>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
/// <summary>
/// Audio
///
/// 概要:
/// - アプリケーション全体で利用するオーディオマネージャ。シングルトンとして設計されXAudio2とMediaFoundationを内部で利用して
///   WAV/MP3等の音声読み込み、再生、停止、音量制御。
/// 
/// 主な機能:
/// - Initialize/Finalize:XAudio2/Media Foundationの初期化と終了処理。
/// - LoadWave/LoadMP3:音声データの読み込みとキャッシュ登録。
/// - Play/Stop/StopAll/Pause/Resume:再生制御。
/// - SetVolume/SetMasterVolume:音量制御。
/// - Update:再生状態の管理やフレーム単位のメンテナンス処理。
/// 
/// 注意:
/// - シングルトンのためグローバルに一つだけ存在する想定。GetInstanceで取得して使用。
/// - Media Foundation や COM の初期化(CoInitializeEx/MFStartup)に依存するため、初期化に失敗した場合は正常に動作しない。
/// - マルチスレッドでの同時操作には注意。
/// </summary>

// 音声データ
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
	// ファイル
	std::string filePath;
	// ファイルの再生時間
	float playTime;
	// 音量
	float volume;
};

struct AudioList
{
	IXAudio2SourceVoice* sourceVoice;
	SoundData soundData;
	XAUDIO2_BUFFER buf{};
	int startFrameTime;
};


class Audio {
private:
	// シングルトンパターンを適用
	static Audio* instance;

	// コンストラクタ、デストラクタの隠蔽
	Audio() = default;
	~Audio() = default;
	// コピーコンストラクタ、コピー代入演算子の封印
	Audio(Audio&) = delete;
	Audio& operator=(Audio&) = delete;

public:

	// 初期化
	void Initialize();

	// 更新
	void Update();

	// インスタンスの取得
	static Audio* GetInstance();

	// 終了処理
	void Finalize();

	// wave読み込み
	bool LoadWave(const std::string filePath, const std::string soundName, const float volume = 1.0f);

	// mp3読み込み
	bool LoadMP3(const std::string filePath, const std::string soundName, const float volume = 1.0f);

	// 音量設定
	void SetVolume(const std::string soundName, const float volume);

	// 主音量設定
	void SetMasterVolume(const float volume);

	// 音声再生
	void Play(const std::string soundName, const bool loop = false);

	//void PlayMp3(const bool loop = false, const float volume = 1.0f);

	// 全ての音声停止
	void StopAll();

	// 音声停止
	void Stop(const std::string soundName);

	// 一時停止
	void Pause(const std::string soundName);

	// 一時停止した音声の再開
	void Resume(const std::string soundName);

	// 音声データ解放
	void SoundUnload(const std::string soundName);

	void Unload(SoundData* soundData);

private:

	// 最大SRV数(最大テクスチャ枚数)
	static const uint32_t maxSourceVoiceCount;

	// audio test
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;

	std::vector<AudioList> audioList;

	std::map<std::string, SoundData> soundMap;

	// オーディオデータ
	std::map<std::string, std::vector<BYTE>> mp3AudioData;
	// WAVEフォーマット
	WAVEFORMATEX mp3waveFormat;

	int frameTime = 0;

	// Media Foundation SourceReader
	Microsoft::WRL::ComPtr<IMFSourceReader> pMFSourceReader{ nullptr };

	// 初期化状態フラグ(CoInitializeEx/MFStartup の成功判定用)
	bool comInitialized = false;
	bool mfStarted = false;
};