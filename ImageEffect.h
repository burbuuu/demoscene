#include "effect.h"
#include <SDL2/SDL_image.h>

class ImageEffect : public Effect {
public:
    ImageEffect(const std::string& imagePath);
    void init() override;
    void update(int currentTime) override;
    void render(SDL_Surface* surface) override;
    void close() override;

private:
    std::string m_imagePath;
    unsigned char* m_grayBuffer;
    RGBColor m_palette[256];
    int m_imgWidth;
    int m_imgHeight;
    int m_shift;

    void buildPalette(int currentTime);
};
