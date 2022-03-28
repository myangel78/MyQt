#ifndef SINGLETON_H
#define SINGLETON_H
#include <memory>
#include <mutex>
#include <atomic>

template<typename  T>
class CSingleton
{
public:
    virtual ~CSingleton(){}
    static T* Instance()
    {
        T* tmp = m_pInstance.load(std::memory_order_acquire);
        if (tmp == nullptr)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            tmp = m_pInstance.load (std::memory_order_relaxed);
            if (tmp == nullptr)
            {
                tmp = new T();
                m_pInstance.store(tmp, std::memory_order_release);
            }
        }
        return tmp;
    }

protected:
    CSingleton(){};

private:
    static std::atomic<T*> m_pInstance;
    static std::mutex m_Mutex;
};

template <typename T> std::atomic<T*> CSingleton<T>::m_pInstance;
template<typename T> std::mutex	CSingleton<T>::m_Mutex;

#endif // SINGLETON_H
