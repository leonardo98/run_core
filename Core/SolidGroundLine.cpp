#include "SolidGroundLine.h"
#include "../Core/Math.h"
#include "../Core/Core.h"
//#include "BikerGame.h"

#define SPLINE_OFFSET 30

void Besier::Path(float t, FPoint2D &pos)
{
    assert(origin.size() >= 4 && ((int)origin.size() - 1) % 3 == 0);
    if (t == 1.f)
    {
        pos = origin.back();
        return;
    }
    assert(0.f <= t && t < 1.f);

    int segmentAmount = origin.size() / 3;
    int segment = static_cast<int>(t * segmentAmount);

    t = t * segmentAmount - segment;

    FPoint2D &p0 = origin[segment * 3];
    FPoint2D &p1 = origin[segment * 3 + 1];
    FPoint2D &p2 = origin[segment * 3 + 2];
    FPoint2D &p3 = origin[segment * 3 + 3];

    pos = ((1 - t) * (1 - t) * (1 - t) * p0
            + 3 * t * (1 - t) * (1 - t) * p1
            + 3 * t * t * (1 - t) * p2
            + t * t * t * p3);
}

float Besier::NearestGradient(const FPoint2D &p, float &gradient)
{
    assert(origin.size() >= 4 && ((int)origin.size() - 1) % 3 == 0);

    float tMin = 0.f;
    FPoint2D pMin, pMax;
    Path(tMin, pMin);
    float tMax = 1.f;
    Path(tMax, pMax);
    FPoint2D m25, m5, m75;
    while ((pMax.x - pMin.x) * (pMax.x - pMin.x) + (pMax.y - pMin.y) * (pMax.y - pMin.y) > 1)
    {
        float t25 = tMin + (tMax - tMin) * 0.25f;
        float t5  = tMin + (tMax - tMin) * 0.5f;
        float t75 = tMin + (tMax - tMin) * 0.75f;
        Path(t25, m25);
        Path(t5, m5);
        Path(t75, m75);
        if ((m25 - p).Length() < (m5 - p).Length() && (m25 - p).Length() < (m75 - p).Length())
        {
            pMax = m5;
            tMax = t5;
        }
        else if ((m5 - p).Length() < (m75 - p).Length())
        {
            pMin = m25;
            tMin = t25;
            pMax = m75;
            tMax = t75;
        }
        else
        {
            pMin = m5;
            tMin = t5;
        }
    }
    //BikerGame::testPoint = pMin;
    gradient = atan2(m75.y - m25.y, m75.x - m25.x);
    return (pMin - p).Length();
    /*
    // ???? ???? ?????, ???????? ??????? ????? ? ????? P
    int result = 0;
    FPoint2D bestM((origin[0] + origin[3]) * 0.5f);
    FPoint2D m;
    for (unsigned int i = 1; i < origin.size() / 3; ++i) {
        m = (origin[i * 3] + origin[i * 3 + 3]) * 0.5f;
        if ((p - bestM).Length() > (p - m).Length())
        {
            bestM = m;
            result = i;
        }
    }

    FPoint2D p0 = origin[result * 3];
    FPoint2D p1 = origin[result * 3 + 1];
    FPoint2D p2 = origin[result * 3 + 2];
    FPoint2D p3 = origin[result * 3 + 3];

    float tMin = 0.f;
    FPoint2D pMin;
    float tMax = 1.f;
    FPoint2D m25, m5, m75;
    while (tMax - tMin > 1e-3)
    {
        float t25 = tMin + (tMax - tMin) * 0.25f;
        float t5  = tMin + (tMax - tMin) * 0.5f;
        float t75 = tMin + (tMax - tMin) * 0.75f;
        m25 = (1 - t25) * (1 - t25) * (1 - t25) * p0
            + 3 * t25 * (1 - t25) * (1 - t25) * p1
            + 3 * t25 * t25 * (1 - t25) * p2
            + t25 * t25 * t25 * p3;
        m5 = (1 - t5) * (1 - t5) * (1 - t5) * p0
            + 3 * t5 * (1 - t5) * (1 - t5) * p1
            + 3 * t5 * t5 * (1 - t5) * p2
            + t5 * t5 * t5 * p3;
        m75 = (1 - t75) * (1 - t75) * (1 - t75) * p0
            + 3 * t75 * (1 - t75) * (1 - t75) * p1
            + 3 * t75 * t75 * (1 - t75) * p2
            + t75 * t75 * t75 * p3;
        if ((m25 - p).Length() < (m5 - p).Length() && (m25 - p).Length() < (m75 - p).Length())
        {
            pMin = m25;
            tMax = t5;
        }
        else if ((m5 - p).Length() < (m75 - p).Length())
        {
            pMin = m5;
            tMin = t25;
            tMax = t75;
        }
        else
        {
            pMin = m75;
            tMin = t5;
        }
    }
    BikerGame::testPoint = pMin;
    gradient = atan2(m75.y - m25.y, m75.x - m25.x);
    return (pMin - p).Length();
    */
}

void Besier::Recalc(std::vector<FPoint2D> &screen, int sectorNumber) const
{
    assert(((int)origin.size() - 1) % 3 == 0);
    screen.clear();
    for (unsigned int i = 0; i < origin.size() / 3; ++i) {
        FPoint2D p0 = origin[i * 3];
        FPoint2D p1 = origin[i * 3 + 1];
        FPoint2D p2 = origin[i * 3 + 2];
        FPoint2D p3 = origin[i * 3 + 3];
        for (int k = 0; k < sectorNumber; ++k) {
            float t = static_cast<float>(k) / sectorNumber;
            screen.push_back((1 - t) * (1 - t) * (1 - t) * p0
                            + 3 * t * (1 - t) * (1 - t) * p1
                            + 3 * t * t * (1 - t) * p2
                            + t * t * t * p3);
        }
    }
    screen.push_back(origin.back());
}

float Besier::Length() const
{
    std::vector<FPoint2D> tmp;
    Recalc(tmp, 10);
    float lenFull = 0.f;
    for (int i = 0; i < static_cast<int>(tmp.size()) - 1; ++i)
    {
        lenFull += (tmp[i] - tmp[i + 1]).Length();
    }
    return lenFull;
}

void Besier::RecalcWithNumber(std::vector<FPoint2D> &screen, int dotNumber) const
{
    screen.clear();

    FPoint2D dot;
    std::vector<FPoint2D> tmp;
    Recalc(tmp, 10);
    float lenFull = 0.f;
    for (int i = 0; i < static_cast<int>(tmp.size()) - 1; ++i)
    {
        lenFull += (tmp[i] - tmp[i + 1]).Length();
    }
    screen.push_back(tmp.front());
    int k = 0;
    float len = 0.f;
    for (int i = 1; i < dotNumber; ++i)
    {
        float searchValue = i * lenFull / dotNumber;
        assert(k + 1 < static_cast<int>(tmp.size()));
        float lenCurrent = (tmp[k + 1] - tmp[k]).Length();
        while (len + lenCurrent < searchValue)
        {
            len += lenCurrent;
            ++k;
            lenCurrent = (tmp[k + 1] - tmp[k]).Length();
        }
        dot = ((searchValue - len) / lenCurrent) * (tmp[k + 1] - tmp[k]) + tmp[k];
        screen.push_back(dot);
    }
    screen.push_back(tmp.back());
}



void SolidGroundLine::ExportToLines(std::vector<FPoint2D> &lineDots, int divisionBy) {
    _besier.Recalc(lineDots, divisionBy);
}

void SolidGroundLine::CreateDots(DotsList &dots) {
    ExportToLines(dots);
    for (unsigned int i = 0; i < dots.size(); ++i) {
        dots[i] = (dots[i] + _pos) * (1.f / SCALE_BOX2D);
    }
}

std::string SolidGroundLine::Type() const {
    return "SolidGroundLine";
}

SolidGroundLine::SolidGroundLine(rapidxml::xml_node<>* xe)
: GroundBase(xe)
{
    bool besier = xe->first_attribute("besier")->value() != NULL;
    rapidxml::xml_attribute<> *size = xe->first_attribute("size");
    if (size)
    {
        _besier.origin.resize(atoi(size->value()));
        std::string array = xe->first_attribute("dots")->value();
        std::string::size_type start = 0;
        std::string::size_type sigma = array.find(";");
        for (unsigned int j = 0; j < _besier.origin.size(); ++j)
        {
            sscanf(array.substr(start, sigma - start).c_str(), "%g %g", &_besier.origin[j].x, &_besier.origin[j].y);
            start = sigma + 1;
            sigma = array.find(";", start);
        }
    }
    else
    {
        rapidxml::xml_node<> *dot = xe->first_node("dot");
        while (dot != NULL) {
            FPoint2D p(static_cast<float>(atof(dot->first_attribute("x")->value())), static_cast<float>(atof(dot->first_attribute("y")->value())));
            if (!besier) {
                if (_besier.origin.size() != 0) {
                    _besier.origin.push_back(_besier.origin.back() + FPoint2D(SPLINE_OFFSET, 0));
                    _besier.origin.push_back(p + FPoint2D(-SPLINE_OFFSET, 0));
                }
                _besier.origin.push_back(p);
            } else {
                _besier.origin.push_back(p);
            }
            dot = dot->next_sibling("dot");
        }
    }
}

FPoint2D SolidGroundLine::GetStart() {
    return _besier.origin.front() + _pos;
}

FPoint2D SolidGroundLine::GetFinish() {
    return _besier.origin.back() + _pos;
}

float SolidGroundLine::NearestGradient(const FPoint2D &p, float &gradient)
{
    float f = _besier.NearestGradient(p * SCALE_BOX2D - _pos, gradient);
    //BikerGame::testPoint += _pos;
    return f;
}

/*
void SolidGroundLine::Draw() {
    assert(((int)_besier.origin.size() - 1) % 3 == 0);

    Render::PushMatrix();
    Render::MatrixMove(_pos.x, _pos.y);

    std::vector<FPoint2D> renderLineDots;
    _besier.Recalc(renderLineDots, 6);
    for (unsigned int i = 1; i < renderLineDots.size(); ++i) {
        Render::Line(renderLineDots[i - 1].x, renderLineDots[i - 1].y
                    , renderLineDots[i].x, renderLineDots[i].y, _color);
    }
    Render::PopMatrix();
}
*/
