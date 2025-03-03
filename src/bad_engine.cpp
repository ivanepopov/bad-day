#include "bad_engine.h"

void BAD_Engine::init(SDL_Renderer* renderer, SDL_Window* window, TTF_Font* font)
{
    this->renderer = renderer;
    this->window = window;
    this->font = font;

    /* load time */
    std::time_t timer = std::time(0);
    localtime = std::localtime(&timer);
    localtime->tm_year += 1900;
    currentYear = localtime->tm_year;

    /* load labels */
    Label leftArrow, rightArrow;
    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };

    SDL_Surface* ls = TTF_RenderText_Blended(font, "<",  0, color);
    SDL_Surface* rs = TTF_RenderText_Blended(font, ">",  0, color);

    leftArrow.texture = SDL_CreateTextureFromSurface(renderer, ls);
    rightArrow.texture = SDL_CreateTextureFromSurface(renderer, rs);

    SDL_DestroySurface(ls);
    SDL_DestroySurface(rs);

    SDL_GetTextureSize(leftArrow.texture, &leftArrow.rect.w, &leftArrow.rect.h);
    leftArrow.rect.x = WINDOW_WIDTH - 95;
    leftArrow.rect.y = WINDOW_HEIGHT - 25;

    SDL_GetTextureSize(rightArrow.texture, &rightArrow.rect.w, &rightArrow.rect.h);
    rightArrow.rect.x = WINDOW_WIDTH - 20;
    rightArrow.rect.y = WINDOW_HEIGHT - 25;

    loadYears();

    /* TODO check if current year is loaded */

    years[localtime->tm_year].months[localtime->tm_mon].days[localtime->tm_mday].color = {255, 69, 0, 255};
}

void BAD_Engine::iterate()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    renderYear(years[currentYear]);
    renderLabels();
    renderHovers();

    /* render that footer line */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderLine(renderer, 0, WINDOW_HEIGHT - 30, WINDOW_WIDTH, WINDOW_HEIGHT - 30);

    SDL_RenderPresent(renderer);
}

void BAD_Engine::quit()
{
    saveYears();
}

void BAD_Engine::mouseInput(const Uint8& button, SDL_FPoint& mousePosition)
{
    for (Month& month : years[currentYear].months)
    {
        for (Day& day : month.days)
        {
            day.hover = 0;
            if (SDL_PointInRectFloat(&mousePosition, &day.rect))
            {
                if (button == SDL_BUTTON_LEFT)
                {
                    day.status++;
                    day.status %= 3;
                }
                else if (button == 0)
                {
                    day.hover = 1;
                }
            }
        }
    }

    /* TODO check if clicked on year arrow keys */
}

void BAD_Engine::loadYears()
{
    const auto dataPath = basePath / yearDataFile;
    std::ifstream data(dataPath);

    std::string line;
    while (data >> line)
    {
        /* TODO error checking */
        int yr = std::stoi(line);
        Year y = createYear(yr);

        for (int i = 0; i < 12; i++)
        {
            data >> line;
            int m = std::stoi(line);
            data >> line;
            for (int j = 0; j < line.size(); j++) y.months[i].days[j].status = line[j] - '0';
        }
        years[yr] = y;
    }
    data.close();
}

void BAD_Engine::saveYears()
{
    const auto dataPath = basePath / yearDataFile;
    std::ofstream data(dataPath);

    for (const auto& [yr, y] : years)
    {
        data << yr << "\n";
        for (int i = 0; i < 12; i++)
        {
            data << i << "\n";
            for (int j = 0; j < y.months[i].days.size(); j++)
            {
                data << y.months[i].days[j].status;
            }
            data << "\n";
        }
    }
    data.close();
}

void BAD_Engine::renderHovers()
{
    for (Month& month : years[currentYear].months)
    {
        for (Day& day : month.days)
        {
            if (day.hover)
            {
                SDL_FRect glow{day.rect.x - 1, day.rect.y - 1, DAY_SIDE + 2, DAY_SIDE + 2};
                SDL_SetRenderDrawColor(renderer, 255, 69, 0, 255);
                SDL_RenderRect(renderer, &glow);

                SDL_FRect message, messageBox;
                SDL_Surface* sf = TTF_RenderText_Blended(font, "Message", 0, {255,69,0,255});
                SDL_Texture* tx = SDL_CreateTextureFromSurface(renderer, sf);
                SDL_DestroySurface(sf);

                messageBox.w = BOX_WIDTH;
                messageBox.h = BOX_HEIGHT;
                messageBox.y = day.rect.y + DAY_SIDE + 5;
                if (messageBox.y + BOX_HEIGHT > WINDOW_HEIGHT) messageBox.y = day.rect.y - 5 - BOX_HEIGHT;
                messageBox.x = day.rect.x;
                if (messageBox.x + BOX_WIDTH > WINDOW_WIDTH) messageBox.x = day.rect.x - BOX_WIDTH + DAY_SIDE;

                SDL_GetTextureSize(tx, &message.w, &message.h);
                message.x = messageBox.x + ((float)BOX_WIDTH / 2) - (message.w / 2);
                message.y = messageBox.y + ((float)BOX_HEIGHT / 2) - (message.y / 2);

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderFillRect(renderer, &messageBox);
                SDL_SetRenderDrawColor(renderer, 255, 69, 0, 255);
                SDL_RenderRect(renderer, &messageBox);

                SDL_RenderTexture(renderer, tx, nullptr, &message);
            }
        }
    }
}

void BAD_Engine::renderLabels()
{
    for (const Label& label : labels)
    {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderTexture(renderer, label.texture, nullptr, &label.rect);
    }
}

void BAD_Engine::renderDay(const Day& day)
{
    SDL_SetRenderDrawColor(renderer, colors[day.status].r, colors[day.status].g, colors[day.status].b, colors[day.status].a);
    switch (day.status)
    {
        case NONE:
        {
            SDL_RenderRect(renderer, &day.rect);
            break;
        }
        case BAD:
        {
            SDL_RenderFillRect(renderer, &day.rect);
            break;
        }
        case HAPPY:
        {
            SDL_RenderFillRect(renderer, &day.rect);
            break;
        }
    }
}

void BAD_Engine::renderMonth(const Month& month)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderTexture(renderer, month.label.texture, nullptr, &month.label.rect);

    for (const Day& day : month.days) renderDay(day);
}

void BAD_Engine::renderYear(const Year& year)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderTexture(renderer, year.label.texture, nullptr, &year.label.rect);

    for (const Month& month : year.months) renderMonth(month);
}

Day BAD_Engine::createDay(SDL_FPoint& start, float side, SDL_Color color)
{
    Day day;

    if (color.r == 0 && color.g == 0 && color.b == 0)
    {
        color.r = (SDL_rand(255) + 100) % 255;
        color.g = (SDL_rand(255) + 100) % 255;
        color.b = (SDL_rand(255) + 100) % 255;
    }

    day.rect = {start.x, start.y, side, side};
    day.color = color;

    return day;
}

Month BAD_Engine::createMonth(SDL_FPoint start, const std::string& month, int days, int offset)
{
    Month m;

    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
    SDL_Surface* sf = TTF_RenderText_Blended(font, month.c_str(), 0, color);
    m.label.texture = SDL_CreateTextureFromSurface(renderer, sf);
    SDL_DestroySurface(sf);

    SDL_GetTextureSize(m.label.texture, &m.label.rect.w, &m.label.rect.h);
    m.label.rect.x = start.x;
    m.label.rect.y = start.y - 20;

    int increment = 20;
    int width = increment * 7;
    int cal_width = start.x + 140;

    start.x += offset * increment;
    for (int i = 1; i <= days; i++)
    {
        m.days.push_back(createDay(start, DAY_SIDE));
        start.x += increment;
        if (start.x >= cal_width) start.x -= width, start.y += increment;
    }

    return m;
}

Year BAD_Engine::createYear(int year)
{
    Year y;

    SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
    SDL_Surface* sf = TTF_RenderText_Blended(font, std::to_string(year).c_str(), 0, color);
    y.label.texture = SDL_CreateTextureFromSurface(renderer, sf);
    SDL_DestroySurface(sf);

    SDL_GetTextureSize(y.label.texture, &y.label.rect.w, &y.label.rect.h);
    y.label.rect.x = WINDOW_WIDTH - 75;
    y.label.rect.y = WINDOW_HEIGHT - 23;

    SDL_FPoint start = {30, 30};
    for (int i = 1, n; i <= 12; i++)
    {
        n = monthInfo[i-1].second;
        if (n == 1 && (year - 2028) % 4 == 0) n++;

        int offset = calcDayOfWeek(year, i, 1, 1)[0] - '0';
        if (offset > 3) offset %= 4; else offset += 3;

        y.months[i-1] = createMonth(start, monthInfo[i-1].first, n, offset);
        start.x += 175;
        if (i % 4 == 0) start.y += 150, start.x = 30;
    }
    return y;
}

std::string BAD_Engine::calcDayOfWeek(unsigned int y, unsigned long m, unsigned long d, bool index)
{
    m = (m + 9) % 12;
    y -= m / 10;
    unsigned long dn = (365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (d - 1)) % 7;
    return index ? std::to_string(dn) : day[dn];
}
