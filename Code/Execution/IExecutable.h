/* Abstract class that serves as a interface for executable objects. */
class IExecutable {

  public:
    virtual ~IExecutable() {}

    /* This function is filled with the code to execute. */
    virtual void Execute() = 0;
};
