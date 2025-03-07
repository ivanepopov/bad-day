#ifndef BAD_ENGINE_H
#define BAD_ENGINE_H

#include "globals.h"
#include <deque>

class BAD_Engine
{
    private:
        SDL_Renderer* renderer;
        SDL_Window* window;
        TTF_Font* font;

        std::tm* localtime;
        int currentYear;
        std::deque<Year> years;
        int loadWidth = 5, loadIndex = -1;

        std::vector<Label> labels;

        std::filesystem::path basePath = "";
        std::filesystem::path yearsPath = "";
        std::string yearDataFile = "bad_day_data.txt";

    public:
        void init(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font);
        void iterate();
        void quit();

        SDL_Renderer*         getRenderer() { return renderer; };
        SDL_Window*           getWindow() { return window; };
        TTF_Font*             getFont() { return font; };
        std::filesystem::path getBasePath() { return basePath; }

        void setRenderer(SDL_Renderer* renderer) { this->renderer = renderer; };
        void setWindow(SDL_Window* window) { this->window = window; };
        void setFont(TTF_Font* font) { this->font = font; };
        void setBasePath(std::filesystem::path path) { basePath = path; }

        void renderLabels();
        void renderHovers();
        void renderDay(const Day& day);
        void renderMonth(const Month& month);
        void renderYear(const Year& year);

        Year loadYear(int cy);
        void saveYear(const Year& cy);

        void loadYears();
        void unloadYears();
        void saveYears();

        Label createLabel(std::string info, float x, float y, SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE});
        Day   createDay(SDL_FPoint& start, float side, SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE});
        Month createMonth(SDL_FPoint start, const std::string& month, int days, int iterate);
        Year  createYear(int year);

        void mouseInput(const Uint8& button, SDL_FPoint& mousePosition);

        /* https://stackoverflow.com/questions/40517192/c-day-of-week-for-given-date */
        std::string calcDayOfWeek(unsigned int y, unsigned long m, unsigned long d, bool index = 0);
};

#endif /* end of include guard: BAD_ENGINE_H */
