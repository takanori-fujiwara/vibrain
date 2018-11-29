#include "roi.hpp"

Roi::Roi() {
  this->name = "";
  this->x = 0.0;
  this->y = 0.0;
  this->z = 0.0;
  this->group = 0;
  this->rank = -1;
  this->visible = true;
  this->size = 0.01;
  this->selected = false;
  this->filteredOut = false;
}

Roi::Roi(QString name, float x, float y, float z, uint group, int rank) {
  this->name = name;
  this->x = x;
  this->y = y;
  this->z = z;
  this->group = group;
  this->rank = rank;
  this->visible = true;
  this->size = 0.01;
  this->selected = false;
  this->filteredOut = false;
}

QString Roi::getName() { return name; }
float Roi::getX() { return x; }
float Roi::getY() { return y; }
float Roi::getZ() { return z; }
uint Roi::getGroup() { return group; }
int Roi::getRank() { return rank; }
bool Roi::getVisible() { return visible; }
float Roi::getSize() { return size; }
bool Roi::getSelected() { return selected; }
bool Roi::getFilteredOut() { return filteredOut; }

void Roi::setGroup(uint group) { this->group = group; }
void Roi::setRank(int rank) { this->rank = rank; }
void Roi::setVisible(bool visible) { this->visible = visible; }
void Roi::setSize(float size) { this->size = size; }
void Roi::setSelected(bool selected) { this->selected = selected; }
void Roi::setFilteredOut(bool filteredOut) { this->filteredOut = filteredOut; }
