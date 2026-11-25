#ifndef DEL_H
#define DEL_H

class DEL {
  private:
    int broche;

  public:
    DEL() {}
    void init(int pin);
    void allumer();
    void eteindre();
    void clignoter();
};

#endif