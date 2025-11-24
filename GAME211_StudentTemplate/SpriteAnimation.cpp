#pragma once
/*
*   Sprites and Animations handling class made by Shayan Hamayun
*/

#define SDL_ENABLE_OLD_NAMES

#include "SpriteAnimation.h"
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <numbers>

namespace fs = std::filesystem;

// Helper: try to extract numeric index from filename, e.g. "survivor-run_12.png" -> 12
static int ExtractFrameIndex(const fs::path& p) {
    std::string name = p.stem().string(); // "survivor-run_12"
    // find last '_' and take what's after
    size_t underscore = name.find_last_of('_');
    if (underscore == std::string::npos || underscore + 1 >= name.size()) {
        return 0; // fallback
    }

    std::string numStr = name.substr(underscore + 1);
    try {
        return std::stoi(numStr);
    }
    catch (...) {
        return 0;
    }
}

SpriteAnimation::SpriteAnimation() :
    frameTime(0.1f),
    currentTime(0.0f),
    currentFrame(0),
    loop(true),
    frameWidth(0),
    frameHeight(0),
    offsetX(0.0f),
    offsetY(0.0f)
{
}

SpriteAnimation::~SpriteAnimation() {
    Clear();
}

void SpriteAnimation::Clear() {
    for (auto tex : frames) {
        if (tex) {
            SDL_DestroyTexture(tex);
        }
    }
    frames.clear();
    currentFrame = 0;
    currentTime = 0.0f;
}

bool SpriteAnimation::LoadFromFolder(SDL_Renderer* renderer,
    const std::string& folderPath,
    float frameTime_,
    bool loop_) {
    Clear();

    frameTime = frameTime_;
    loop = loop_;
    currentTime = 0.0f;
    currentFrame = 0;

    std::vector<fs::path> imageFiles;
    try {
        for (auto& entry : fs::directory_iterator(folderPath)) {
            if (!entry.is_regular_file()) continue;
            auto p = entry.path();
            if (p.extension() == ".png" || p.extension() == ".PNG") {
                imageFiles.push_back(p);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "SpriteAnimation: error reading folder "
            << folderPath << " : " << e.what() << std::endl;
        return false;
    }

    if (imageFiles.empty()) {
        std::cerr << "SpriteAnimation: no PNG files found in "
            << folderPath << std::endl;
        return false;
    }

    // Sort files by numeric frame index so frames are in proper order
    std::sort(imageFiles.begin(), imageFiles.end(),
        [](const fs::path& a, const fs::path& b) {
            int ia = ExtractFrameIndex(a);
            int ib = ExtractFrameIndex(b);
            if (ia != ib) return ia < ib;
            // fallback: lexicographic if numbers equal / missing
            return a.filename().string() < b.filename().string();
        });

    // load each image into frames
    for (const auto& path : imageFiles) {
        SDL_Surface* surf = IMG_Load(path.string().c_str());
        if (!surf) {
            std::cerr << "SpriteAnimation: IMG_Load failed for "
                << path << std::endl;
            continue;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        if (!tex) {
            std::cerr << "SpriteAnimation: SDL_CreateTextureFromSurface failed: "
                << SDL_GetError() << std::endl;
            SDL_FreeSurface(surf);
            continue;
        }

        if (frames.empty()) {
            frameWidth = surf->w;
            frameHeight = surf->h;
        }

        SDL_FreeSurface(surf);
        frames.push_back(tex);
    }

    if (frames.empty()) {
        std::cerr << "SpriteAnimation: no textures loaded from "
            << folderPath << std::endl;
        return false;
    }

    return true;
}

void SpriteAnimation::Update(float deltaTime) {
    // If there's 0 or 1 frame, don't animate – just keep frame 0.
    if (frames.size() <= 1) return;

    currentTime += deltaTime;

    while (currentTime >= frameTime) {
        currentTime -= frameTime;
        currentFrame++;

        if (currentFrame >= static_cast<int>(frames.size())) {
            if (loop) {
                currentFrame = 0;
            }
            else {
                currentFrame = static_cast<int>(frames.size()) - 1;
                break;
            }
        }
    }
}

void SpriteAnimation::Render(SDL_Renderer* renderer,
    float x, float y, float scale) const {
    if (frames.empty()) return;

    SDL_Texture* tex = frames[currentFrame];

    SDL_FRect dst;
    dst.w = frameWidth * scale;
    dst.h = frameHeight * scale;

    float cx = x + offsetX * scale;
    float cy = y + offsetY * scale;

    dst.x = cx - 0.5f * dst.w;
    dst.y = cy - 0.5f * dst.h;

    SDL_RenderCopyEx(renderer, tex, nullptr, &dst,
        0.0, nullptr, SDL_FLIP_NONE);
}


void SpriteAnimation::RenderRotated(SDL_Renderer* renderer,
    float x, float y,
    float scale, float angleDegrees) const {
    if (frames.empty()) return;

    SDL_Texture* tex = frames[currentFrame];

    SDL_FRect dst;
    dst.w = frameWidth * scale;
    dst.h = frameHeight * scale;

    // Half-sizes
    float halfW = dst.w * 0.5f;
    float halfH = dst.h * 0.5f;

    // Interpret offsetX/offsetY as *local* sprite-space pixels (unscaled)
    float offLocalX = offsetX * scale;
    float offLocalY = offsetY * scale;

    // Convert angle to radians
    float rad = angleDegrees * static_cast<float>(std::numbers::pi) / 180.0f;

    // Rotate the local offset by the player orientation,
    // so the offset moves with the aim direction.
    float cosA = std::cos(rad);
    float sinA = std::sin(rad);

    float offWorldX = offLocalX * cosA - offLocalY * sinA;
    float offWorldY = offLocalX * sinA + offLocalY * cosA;

    // Final sprite center = player position + rotated offset
    float cx = x + offWorldX;
    float cy = y + offWorldY;

    // Place rect so that its center is at (cx, cy)
    dst.x = cx - halfW;
    dst.y = cy - halfH;

    // Rotate around the visual center of the sprite
    SDL_FPoint center;
    center.x = halfW;
    center.y = halfH;

    SDL_RenderCopyEx(renderer, tex, nullptr, &dst,
        angleDegrees, &center, SDL_FLIP_NONE);
}

void SpriteAnimation::Reset() {
    currentTime = 0.0f;
    currentFrame = 0;
}