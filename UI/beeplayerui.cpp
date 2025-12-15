/*  Copyright (c) 2025 Romi Brooks <qq1694821929@gmail.com>
 *  File Name: beeplayerui.cpp
 *  Lib: Beeplayer Qt UI Source file
 *  Author: Romi Brooks
 *  Date: 2025-07-05
 *  Type: UI, GUI, Qt
 */

#include "beeplayerui.h"
#include "ui_beeplayerui.h"

// Basic File
#include "../Log/LogSystem.hpp"
\
// QtLib
#include <QStringListModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QPainter>
#include <QPainterPath>
#include <QStandardItemModel>
#include <QTimer>
#include <QtConcurrent/QtConcurrent>
#include <QFontDatabase>

// we use this to build an ui, and usually explicit passby the root path to provide that PlayerController can be workfine.
BeeplayerUI::BeeplayerUI(QWidget *parent, std::string RootPath)
    : QMainWindow(parent)
    , ui(new Ui::BeeplayerUI), controller(new PlayerController())
{
    ui->setupUi(this);
    this->BasicInit(); // Set application's icon and title

    if(RootPath == "") { // if we don't get that root path, use ui to make sure PlayerController can init property.
        connect(ui->SelectorBrowse, &QPushButton::clicked, this, &BeeplayerUI::onBrowseButtonClicked); // Give me an Explorer.exe invoke
        connect(ui->SelectorSubmit, &QPushButton::clicked, this, &BeeplayerUI::onPathSubmitted); // Pass the root Path to PlayerController, or input by your own
        // ui->MainUI->setVisible(false); // Hide the Beeplayer Main Windows
    } else{
        if(this->controller->Initialize(RootPath)) {// If init is ok
            this->RenderSongList(); // render the Player List for Beeplayer Main Windows
            Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_PATH, "Set Path by -root:", RootPath);
            ui->HeaderPanel->setVisible(false);
            this->isSetPath = true;
            this->SetSongName();
            this->UpdateAlbumArt();
            progressTimer->start(); // 启动进度更新定时器
        }
    }

    // 歌曲切换回调
    this->controller->SetTrackChangeCallback([this](size_t newIndex) {
        QMetaObject::invokeMethod(this, [this, newIndex]() {
            this->UpdateTrackInfo(newIndex);
        }, Qt::QueuedConnection);
    });

    // List Switch Song
    connect(ui->SongList, &QListView::doubleClicked,
            this, &BeeplayerUI::on_SongList_doubleClicked);

    // Progress
    connect(progressWidget, &ProgressWidget::seekRequested,
            this, &BeeplayerUI::OnSeekRequested);

    // Progress Updater
    connect(progressTimer, &QTimer::timeout,
            this, &BeeplayerUI::UpdatePlaybackProgress);

    // Volume
    connect(volumeSlider, &VolumeSlider::volumeChanged,
            this, &BeeplayerUI::onVolumeChanged);
}

// Basiclly Init , we set the title and icon, then import css
void BeeplayerUI::BasicInit() {
    // Font Setting
    // 1. 加载并注册自定义字体
    int fontId = QFontDatabase::addApplicationFont(":/font/msyh");
    if (fontId == -1) {
        qWarning() << "Failed to load application font!";
        globalFontFamily = "Segoe UI"; // 使用备用字体
    } else {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            globalFontFamily = fontFamilies.at(0);
            qDebug() << "Loaded font family:" << globalFontFamily;
        } else {
            globalFontFamily = "Segoe UI";
        }
    }

    // 3. 设置应用程序全局字体
    QFont globalFont(globalFontFamily);
    globalFont.setPointSize(9); // 设置默认字号
    QApplication::setFont(globalFont);

    // Basic Setting
    this->setWindowTitle("Beeplayer");
    this->setWindowIcon(QIcon(":/icon/ico"));
    this->LoadStyleSheet(":/style/style");

    // Album Setting
    m_albumArt = qobject_cast<RotatingAlbumArt*>(ui->SongAvator);
    if (!m_albumArt) {
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_QT,
                    "Faied to get RotatingAlbumArt instance");
    }
    QPixmap albumArt(":/player/cd");
    this->m_albumArt->setPixmap(albumArt);
    Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_QT,
                "Load defualt Album Art.");
    this->UpdateAlbumArt();

    // Progress Setting
    progressTimer = new QTimer(this);
    progressTimer->setInterval(100); // 每100毫秒更新一次进度
    progressWidget = new ProgressWidget(this);
    ui->playerLayout->insertWidget(6, progressWidget); // 插入到合适的位置

    // Non-Block Setting
    seekDebounceTimer = new QTimer(this);
    seekDebounceTimer->setSingleShot(true);

    // Volume Setting
    this->RenderVolumeSlider();
}

BeeplayerUI::~BeeplayerUI()
{
    delete ui;
    delete controller;
}

// Custom Fucntions
// We use std::vector (our Path Lib's API Type) trans to QString(QtLib's API Type) then render to QListView entity
void BeeplayerUI::RenderSongList() {
    // Get data via PathLib's API
    std::vector<std::string> SongList = this->controller->GetTracks();

    QListView *ListView = ui->SongList;

    // 将 std::string 转换为 QString
    QStringList stringList;
    for (const auto& song : SongList) {
        stringList.append(QString::fromStdString(song));
    }

    // 创建字符串列表模型并设置数据
    QStringListModel *model = new QStringListModel(stringList, this);

    // 将模型设置给 ListView
    ListView->setModel(model);
    ui->SongList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // hidden the slider
}

void BeeplayerUI::RenderVolumeSlider() {
    // 创建音量滑块
    volumeSlider = new VolumeSlider(this); // 或者 new QSlider(Qt::Horizontal, this);
    volumeSlider->setObjectName("volumeSlider");

    // 添加到UI布局（根据您的实际布局位置）
    ui->volumeLayout->addWidget(volumeSlider);

    // 初始化音量
    float initialVolume = controller->GetVolume();
    volumeSlider->setValue(static_cast<int>(initialVolume * 100));
}

void BeeplayerUI::SetSongName() {

    // 获取原始文件名（带后缀）
    std::string fullName = this->controller->GetCurrentTrackName();
    QString fileName = QString::fromStdString(fullName);

    // 移除文件后缀
    int lastDotIndex = fileName.lastIndexOf('.');
    if (lastDotIndex > 0) {  // 确保点号不是第一个字符（如隐藏文件）
        fileName = fileName.left(lastDotIndex);
    }

    ui->SongTitle->setText(fileName);
    ui->SongProducer->setText(QString(this->controller->GetCurrentTrackProducer().c_str()));
    // 确保应用样式表
    ui->SongTitle->style()->polish(ui->SongTitle);
    ui->SongProducer->style()->polish(ui->SongProducer);
}

void BeeplayerUI::SetupScrollLabels()
{
    ui->SongProducer->setScrollSpeed(60);
}

void BeeplayerUI::LoadStyleSheet(const QString &path) {
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString styleSheet = QLatin1String(file.readAll());
        qApp->setStyleSheet(styleSheet);
        file.close();
        Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_QT, "Success to load the css style:", path.toStdString());
    }
    else
    {
        Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_QT, "Error to load the css style:", path.toStdString());
    }
}

void BeeplayerUI::UpdateAlbumArt() {
    // 从Controller获取专辑封面数据
    const std::vector<unsigned char> imageData = this->controller->GetCurrentTrackAlbum();
    SetAlbumArt(imageData);
}

void BeeplayerUI::SetAlbumArt(const std::vector<unsigned char>& imageData)
{
    if (imageData.empty()) {
        QPixmap albumArt(":/player/cd");
        m_albumArt->setPixmap(albumArt);
        return;
    }

    // 从二进制数据创建 QPixmap
    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData.data(), static_cast<uint>(imageData.size()))) {
        // 加载失败时显示错误提示
        QPixmap albumArt(":/player/cd");
        m_albumArt->setPixmap(albumArt);
        return;
    }

    // 创建圆形专辑封面
    int size = qMin(ui->SongAvator->width(), ui->SongAvator->height());
    QPixmap rounded = RoundedPixmap(pixmap, size);

    // 设置控件显示
    ui->SongAvator->setPixmap(rounded);
    ui->SongAvator->setStyleSheet(""); // 清除文本样式
}

void BeeplayerUI::UpdateTrackInfo(size_t trackIndex)
{
    // 更新歌曲标题
    this->SetSongName();

    // 更新制作人
    ui->SongProducer->setText(
        QString::fromStdString(controller->GetCurrentTrackProducer())
        );

    // 更新专辑封面
    this->m_albumArt->stopRotation();
    std::vector<unsigned char> coverData = controller->GetCurrentTrackAlbum();
    SetAlbumArt(coverData);
    this->m_albumArt->resumeRotation();

    // 更新列表选中项
    if (ui->SongList->model()) {
        QModelIndex modelIndex = ui->SongList->model()->index(static_cast<int>(trackIndex), 0);
        ui->SongList->setCurrentIndex(modelIndex);
        ui->SongList->scrollTo(modelIndex);
    }
}

QPixmap BeeplayerUI::RoundedPixmap(const QPixmap& source, int size)
{
    // 创建目标图像
    QPixmap target(size, size);
    target.fill(Qt::transparent);

    // 创建圆形剪裁区域
    QPainter painter(&target);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // 绘制圆形路径
    QPainterPath path;
    path.addEllipse(0, 0, size, size);
    painter.setClipPath(path);

    // 缩放并居中绘制图像
    QPixmap scaled = source.scaled(size, size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    painter.drawPixmap(0, 0, size, size, scaled);

    return target;
}

void BeeplayerUI::UpdatePlaybackProgress() {
    if (!controller || !controller->IsInitialized() ||
        !controller->IsPlaying() || isSeeking) {
        return;
    }

    // 获取当前进度和时间
    float currentTime = controller->GetCurrentTime();
    float totalTime = controller->GetTotalTime();
    float progress = controller->GetCurrentProgress();

    // 更新进度控件显示
    progressWidget->setCurrentTime(currentTime);
    progressWidget->setTotalTime(totalTime);
    progressWidget->setProgress(progress);
}

void BeeplayerUI::OnSeekRequested(float progress) {
    if (!controller || !controller->IsInitialized() || isSeeking) {
        return;
    }

    Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_QT, "Seek requested to:" , progress * 100 , "%");

    // 更新预览显示
    float totalTime = controller->GetTotalTime();
    progressWidget->setCurrentTime(progress * totalTime);

    // 暂停自动进度更新
    progressTimer->stop();

    // 设置跳转标志
    isSeeking = true;

    // 使用定时器延迟跳转（防抖）
    seekDebounceTimer->disconnect();
    connect(seekDebounceTimer, &QTimer::timeout, this, [this, progress]() {
        // 在实际跳转前再次检查控制器状态
        if (!controller || !controller->IsInitialized()) {
            isSeeking = false;
            return;
        }

        // 执行跳转操作
        controller->SeekToPosition(progress);

        // 手动更新一次进度
        QMetaObject::invokeMethod(this, [this]() {
            UpdatePlaybackProgress();
            isSeeking = false;

            // 如果正在播放，恢复进度更新
            if (controller->IsPlaying()) {
                progressTimer->start();
            }
        });
    });

    // 设置防抖时间（150ms）
    seekDebounceTimer->start(350);
}

// Slot Functions
// 这里是直接和Qt API相互通讯的接口实现
// Give user an explorer.exe window which can choose the path easily
void BeeplayerUI::onBrowseButtonClicked() {
    QString path = QFileDialog::getExistingDirectory(this, "选择歌曲目录");
    if (!path.isEmpty()) {
        ui->SelectorPathLine->setText(path);
    }
}

// Submit the input path and init the PlayerController
// The proc logic like our Main Constructor
void BeeplayerUI::onPathSubmitted() {
    QString path = ui->SelectorPathLine->text();

    if (!path.isEmpty()) {
        currentSongPath = path.toStdString();

        // Safety Check
        if (!this->controller) {
            Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_QT, "Controller does not init.");
            return;
        }

        if (this->controller->Initialize(currentSongPath)) {
            // ui->MainUI->setVisible(true);
            // ui->HeaderPanel->setVisible(false);
            this->isSetPath = true;
            ui->HeaderPanel->setVisible(false);
            this->RenderSongList();
            Log::LogOut(LogLevel::BP_INFO, LogChannel::CH_QT, "Set Path with QtUI:", currentSongPath);
            this->SetSongName();
            this->UpdateAlbumArt();
            progressTimer->start(); // 启动进度更新定时器
        } else {
            Log::LogOut(LogLevel::BP_ERROR, LogChannel::CH_QT, "Error to set Path with QtUI:", currentSongPath);
        }
    } else {
        Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_QT, "Empty Path");
    }
}

// Slot: Player Controll btn
void BeeplayerUI::on_PlayControlBtn_clicked()
{

    if (!controller->IsInitialized()) {
        Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_CONTROLLER,
                    "Try to Play without Controller init.");
        return;
    }

    if (!controller->IsPlaying()) {
        controller->Play();
        this->SetSongName();
        progressTimer->start(); // 启动进度更新定时器
        ui->PlayControlBtn->setIcon(QIcon(":/controller/pause"));
        if (m_albumArt) {
            m_albumArt->resumeRotation();
        }
    } else {
        controller->Pause();
        progressTimer->stop(); // 停止进度更新定时器
        ui->PlayControlBtn->setIcon(QIcon(":/controller/play"));
        if (m_albumArt) {
            m_albumArt->pauseRotation();
        }
    }
}

void BeeplayerUI::on_PlayNextBtn_clicked()
{
    if (!controller->IsInitialized()) {
        Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_CONTROLLER,
                    "Try to Play Next without Controller init.");
        return;
    }
    this->controller->Next();
    this->SetSongName();
    this->UpdateAlbumArt();
    this->m_albumArt->stopRotation();
    progressTimer->stop(); // 停止进度更新定时器
    controller->Play();
    ui->PlayControlBtn->setIcon(QIcon(":/controller/pause"));
    this->m_albumArt->resumeRotation();
    progressTimer->start(); // 启动进度更新定时器

}

void BeeplayerUI::on_PlayPrevBtn_clicked()
{
    if (!controller->IsInitialized()) {
        Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_CONTROLLER,
                    "Try to Play Prev without Controller init.");
        return;
    }

    this->controller->Prev();
    this->SetSongName();
    this->on_PlayControlBtn_clicked();
    ui->PlayControlBtn->setIcon(QIcon(":/controller/pause"));
    this->m_albumArt->stopRotation();
    this->UpdateAlbumArt();
    this->m_albumArt->resumeRotation();
}

void BeeplayerUI::on_PlayStopBtn_clicked()
{
    if (!controller->IsInitialized()) {
        Log::LogOut(LogLevel::BP_WARNING, LogChannel::CH_CONTROLLER,
                    "Try to Stop without Controller init.");
        return;
    }
    controller->Stop();
    progressTimer->stop();
    progressWidget->setProgress(0);
    progressWidget->setCurrentTime(0);
    ui->PlayControlBtn->setIcon(QIcon(":/controller/play"));
    if (m_albumArt) {
        m_albumArt->stopRotation();
    }
}


void BeeplayerUI::on_SongList_doubleClicked(const QModelIndex &index)
{
    if(this->controller->IsPlaying() == true) {
        controller->Stop();
    }

    int clickedIndex = index.row();

    size_t trackIndex = static_cast<size_t>(clickedIndex);

    controller->Switch(trackIndex);
    this->on_PlayControlBtn_clicked();
    ui->SongList->setCurrentIndex(index);
    ui->SongList->scrollTo(index);
}

void BeeplayerUI::onVolumeChanged(float volume)
{
    // 更新控制器音量
    controller->SetVolume(volume);
}
