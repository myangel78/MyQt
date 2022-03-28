#ifndef EXECUTELISTENER_H
#define EXECUTELISTENER_H

class ExecuteListener
{
public:
    virtual ~ExecuteListener(){}
    /*
     * @param id: mark who triggered the current callback
     * @param status: refs DeviceStatus
    */
    virtual void OnExecuteStatusNotify(int id, int status) = 0;
};

#endif // IONEXECUTELISTENER_H
