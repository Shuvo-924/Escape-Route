#include "MiniMap.h"

Minimap::Minimap()
{
    playerDotTexture.loadFromFile("Position.png");
    playerDotSprite.setTexture(playerDotTexture);
    playerDotSprite.setScale(0.025f, 0.025f);
    playerDotSprite.setOrigin(playerDotSprite.getLocalBounds().width / 2.f, playerDotSprite.getLocalBounds().height / 2.f);

    minimapView.setSize(960, 590); //Set to a portion of your full map for a zoomed-in effect
    minimapView.setViewport(FloatRect(0.75f, 0.05f, 0.2f, 0.2f)); // Top-right corner

    // Configure minimap border
    minimapBorder.setSize(Vector2f(1920 * 0.2f, 1080 * 0.2f));
    minimapBorder.setPosition(1920 * 0.75f, 1080 * 0.05f);
    minimapBorder.setOutlineColor(Color::White);
    minimapBorder.setOutlineThickness(2.f);
    minimapBorder.setFillColor(Color::Transparent);
}

void Minimap::expand()
{
    minimapView.setSize(1920, 1080);
    minimapView.setViewport(FloatRect(0.025f, 0.025f, 0.92f, 0.92f));
    minimapBorder.setSize(Vector2f(1920 * 0.92f, 1080 * 0.92f));
    minimapBorder.setPosition(1920 * 0.025f, 1080 * 0.025f);
}

void Minimap::reset() {
    minimapView.setSize(960, 590);
    minimapView.setViewport(FloatRect(0.75f, 0.05f, 0.2f, 0.2f));
    minimapBorder.setSize(Vector2f(1920 * 0.2f, 1080 * 0.2f));
    minimapBorder.setPosition(1920 * 0.75f, 1080 * 0.05f);
}

void Minimap::setcenter(float x, float y)
{
    const Vector2f mapSize(1920.0f, 1080.0f);
    Vector2f viewSize = minimapView.getSize();

    Vector2f desiredCenter(x, y);

    Vector2f finalCenter = desiredCenter;

    float minX = viewSize.x / 2.0f + 2.f;
    float maxX = (mapSize.x - viewSize.x / 2.0f) - 2.f;
    finalCenter.x = max(minX, min(finalCenter.x, maxX));

    float minY = viewSize.y / 2.0f + 2.f;
    float maxY = (mapSize.y - viewSize.y / 2.0f) - 2.f;
    finalCenter.y = max(minY, min(finalCenter.y, maxY));

    minimapView.setCenter(finalCenter);
}
