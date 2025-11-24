/*
*   Sprites and Animations handling class made by Shayan Hamayun
*/

#ifndef SPRITE_ANIMATION_H
#define SPRITE_ANIMATION_H

#define SDL_ENABLE_OLD_NAMES

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

class SpriteAnimation {
public:
    SpriteAnimation();
    ~SpriteAnimation();

    /// Load all .png images from a folder as frames.
    /// frameTime_ = seconds per frame, loop_ = should the animation loop?
    bool LoadFromFolder(SDL_Renderer* renderer,
        const std::string& folderPath,
        float frameTime_,
        bool loop_ = true);

    void Update(float deltaTime);

    /// Render centered at (x, y)
    void Render(SDL_Renderer* renderer, float x, float y, float scale = 1.0f) const;

    /// Render centered at (x, y) with rotation (degrees)
    void RenderRotated(SDL_Renderer* renderer, float x, float y,
        float scale, float angleDegrees) const;

    void Reset();

    bool IsValid() const { return !frames.empty(); }

    // GETTERS
    int   GetFrameCount() const { return static_cast<int>(frames.size()); }
    float GetFrameTime() const { return frameTime; }

    // SETTERS
    void SetOffsetPixels(float ox, float oy) {
        offsetX = ox;
        offsetY = oy;
    }

private:
    std::vector<SDL_Texture*> frames;
    float frameTime;        // time per frame
    float currentTime;      // time accumulated for current frame
    int   currentFrame;
    bool  loop;

    int frameWidth;
    int frameHeight;

    float offsetX;
    float offsetY;

    void Clear();
};

#endif
