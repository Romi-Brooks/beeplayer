/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: beeplayerui.hpp
 *  Lib: Beeplayer Qt UI Source file's definitions
 *  Author: Romi Brooks
 *  Date: 2025-07-05
 *  Type: UI, GUI, Qt
 */

#ifndef BEEPLAYERUI_H
#define BEEPLAYERUI_H

#include <QMainWindow>
#include "../Engine/Controller.hpp"
#include "Animations/rotatingalbumart.h"
#include "volumeslider.h"
#include "progresswidget.h"

namespace Ui {
class BeeplayerUI;
}

class BeeplayerUI : public QMainWindow
{
    Q_OBJECT

public:
    BeeplayerUI(QWidget *parent = nullptr, std::string SongPath = "");
    ~BeeplayerUI();
    void BasicInit();
    void LoadStyleSheet(const QString &path);
    void RenderSongList();
    void RenderVolumeSlider();
    void SetSongName();
    void SetupScrollLabels();

    void UpdateAlbumArt();
    void UpdatePlaybackProgress();

    void OnSeekRequested(float progress);

    void SetAlbumArt(const std::vector<unsigned char>& imageData);

    void UpdateTrackInfo(size_t trackIndex);

    auto GetController();

private slots:
    void onBrowseButtonClicked();

    void onPathSubmitted();

    void on_PlayControlBtn_clicked();

    void on_PlayNextBtn_clicked();

    void on_PlayPrevBtn_clicked();

    void on_PlayStopBtn_clicked();

    void on_SongList_doubleClicked(const QModelIndex &index);

    void onVolumeChanged(float volume);

private:
    Ui::BeeplayerUI *ui;
    // Font
    QString globalFontFamily;

    // Controller
    PlayerController *controller = nullptr;
    std::string currentSongPath;
    bool isSetPath = false;

    // Volume
    VolumeSlider *volumeSlider;

    // Progress
    QTimer *progressTimer;
    ProgressWidget *progressWidget;

    // Non-Block proc
    QTimer *seekDebounceTimer;
    bool isSeeking = false;


    // For Album Rotation Functions
    RotatingAlbumArt* m_albumArt;
    QPixmap RoundedPixmap(const QPixmap& source, int radius);
};

#endif // BEEPLAYERUI_H
