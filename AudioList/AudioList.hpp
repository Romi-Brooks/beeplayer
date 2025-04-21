// #ifndef AUDIOLIST_HPP
// #define AUDIOLIST_HPP
// #include <vector>
// #include <string>
// #include <functional>
//
// class Playlist {
// public:
//     struct Track {
//         std::string path;
//         std::string title;
//         std::string artist;
//         double duration;
//     };
//
//     void addTrack(Track track);
//     void removeTrack(size_t index);
//     void clear();
//
//     const Track& currentTrack() const;
//     const Track& nextTrack(PlayMode mode);
//     const Track& previousTrack(PlayMode mode);
//
//     void setPlayMode(PlayMode mode);
//     void shuffle();
//
// private:
//     std::vector<Track> tracks_;
//     size_t currentIndex_ = 0;
//     PlayMode mode_ = PlayMode::Sequential;
// };
//
// #endif //AUDIOLIST_HPP
