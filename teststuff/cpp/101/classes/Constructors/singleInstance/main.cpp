




class childClass {
  ParentClass &parent_instance;
  public:
  childClass(ParentClass &parent, string var) : parent_instance(parent) {
    /*constructor stuff*/
  }
  void useChild();
};