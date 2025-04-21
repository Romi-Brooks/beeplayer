// //
// // Created by Romi on 25-4-5.
// //
//
// #ifndef PLAYERCONTROLLER_HPP
// #define PLAYERCONTROLLER_HPP
//
//
// class PlayerController {
// public:
//     PlaybackController(std::shared_ptr<AudioPlayer> player,
//                       std::shared_ptr<Playlist> playlist);
//
//     void play();
//     void pause();
//     void stop();
//     void next();
//     void previous();
//
//     void togglePlayPause();
//     void seekForward(double seconds);
//     void seekBackward(double seconds);
//
//     // 状态观察者接口
//     class Observer {
//     public:
//         virtual void onStateChanged(AudioPlayer::State) = 0;
//         virtual void onTrackChanged(const Playlist::Track&) = 0;
//     };
//     void addObserver(std::weak_ptr<Observer> observer);
//
// private:
//     std::shared_ptr<AudioPlayer> player_;
//     std::shared_ptr<Playlist> playlist_;
//     std::vector<std::weak_ptr<Observer>> observers_;
//
//     void notifyStateChange();
//     void notifyTrackChange();
// };
//
//
// #endif //PLAYERCONTROLLER_HPP
