#ifndef ROI_H
#define ROI_H

#include <QString>

class Roi
{
public:
    Roi();
    Roi(QString name, float x, float y, float z, uint group = 0, int rank = -1);
    QString getName();
    float getX();
    float getY();
    float getZ();
    uint getGroup();
    int getRank();
    bool getVisible();
    float getSize();
    bool getSelected();
    bool getFilteredOut();
    void setGroup(uint group);
    void setRank(int rank);
    void setVisible(bool visible);
    void setSize(float size);
    void setSelected(bool selected);
    void setFilteredOut(bool filteredOut);

private:
    QString name;
    float x;
    float y;
    float z;
    uint group;
    int rank;

    // TODO: maybe these should be separated from this class
    bool visible;
    float size;
    bool selected;
    bool filteredOut;
};

#endif // ROI_H
