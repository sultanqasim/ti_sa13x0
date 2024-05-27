/* --COPYRIGHT--, 
Permissive via Author: Emad Barsoum
15. September 2011
 * --/COPYRIGHT--*/
/*! \file cThread.h */
#pragma once
#include <windows.h>
#include <process.h>

using namespace std;

/*!
 \brief Add brief

 \enum eTaskState
*/
enum eTaskState{
    INIT,     /*!< Add in-line comment */
    READY,    /*!< Add in-line comment */
    RUN,      /*!< Add in-line comment */
    STOP,     /*!< Add in-line comment */
    CLOSEING, /*!< Add in-line comment */
    FAULT     /*!< Add in-line comment */
};

//-- Begin of CThread Declaration --

/*!
 \brief Add brief

 \class cThread cThread.h "cThread.h"
*/
class cThread
{
public:
    /*!
     \brief Constructor

     \param nPriority Add param
    */
    cThread(int nPriority = THREAD_PRIORITY_NORMAL);

    /*!
     \brief Start the thread or recreate it, if it has been terminated before

     \return bool
    */
    bool Start(void);
    /*!
     \brief Start the thread and return when it actualy start

     \return bool
    */
    bool StartAndWait(void);
    /*!
     \brief Pause the thread

     \return bool
    */
    bool Pause(void);
    /*!
     \brief Check if the thread is running or not

     \return bool
    */
    bool IsRunning(void);
    /*!
     \brief Check if the thread has been terminated or not

     \return bool
    */
    bool IsTerminated(void);
    /*!
     \brief Check for the thread is suspend or not

     \return bool
    */
    bool IsSuspend(void);
    /*!
     \brief Terminate immediate the thread - Unsafe

    */
    void Terminate(void);
    /*!
     \brief Wait until the thread terminate

       After this function you are sure that the thread is terminated

     \param dwMiliSec Add param
     \return bool
    */
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    /*!
     \brief Set thread priority

     \param nLevel Add param
    */
    void SetPriority(int nLevel);
    /*!
     \brief Get thread priority

     \return int
    */
    int GetPriority(void);
    /*!
     \brief Speed up thread execution - increase priority level

    */
    void SpeedUp(void);
    /*!
     \brief Slow down Thread execution - decrease priority level

    */
    void SlowDown(void);

protected:
    /*!
     \brief Destructor

    */
    virtual ~cThread()
    {
        ::CloseHandle(m_hEvent);
    }
    /*!
     \brief Put the initialization code here

    */
    virtual void OnInitInstance(void){}
    /*!
     \brief Put the main code of the thread here

        Must be overloaded

    */
    virtual void OnRunning(void) = 0;
    /*!
     \brief Put the cleanup code here

     \return DWORD
    */
    virtual DWORD OnExitInstance(void){return 0;}
    /*!
     \brief Exit the thread safety

    */
    void Exit(void);
    /*!
     \brief Thread function

     \param lpParameter Add param
     \return unsigned __stdcall
    */
    static unsigned __stdcall _ThreadProc(LPVOID lpParameter);

protected:
    HANDLE m_hThread,          /*!< Thread Handle */
           m_hEvent;           /*!< Event handle */
    int m_nInitPriority;       /*!< Add in-line comment */
    unsigned int m_dwThreadID; /*!< Contain the thread ID */
    bool m_bTerminate,         /*!< Thread state is terminated */
         m_bSuspend,           /*!< Thread state is suspended */
         m_bIsRunning;         /*!< Thread state is running */
};

//-- End of CThread Class Declaration --

//-- Begin of TThread Declaration --

/*!
 \brief Template Thread Class

 \class TThread cThread.h "cThread.h"
 \tparam T Thread
*/
template<typename T>
class TThread
{
public:
    /*!
     \brief The constructor

     \param thObject Add param
     \param (*pfnOnRunning)() Add param
     \param nPriority Add param
    */
    TThread(T& thObject, void (T::*pfnOnRunning)(), int nPriority = THREAD_PRIORITY_NORMAL);
    /*!
     \brief Destructor

    */
    virtual ~TThread()
    {
        ::CloseHandle(m_hEvent);
    }

    /*!
     \brief Wait until the thread terminate, after this function you are sure that the thread is terminated

     \param dwMiliSec Add param
     \return bool
    */
    bool WaitUntilTerminate(DWORD dwMiliSec = INFINITE);
    /*!
    \brief Start the thread or recreate it, if it has been terminated before

     \return bool
    */
    bool Start(void);
    /*!
     \brief Start the thread and return when it actualy start

     \return bool
    */
    bool StartAndWait(void);
    /*!
     \brief Pause the thread

     \return bool
    */
    bool Pause(void);
    /*!
     \brief Check if the thread is running or not

     \return bool
    */
    bool IsRunning(void);
    /*!
     \brief Check if the thread has been terminated or not

     \return bool
    */
    bool IsTerminated(void);
    /*!
     \brief Check for the thread is suspend or not

     \return bool
    */
    bool IsSuspend(void);
    /*!
     \brief Set thread priority

     \param nLevel Add param
    */
    void SetPriority(int nLevel);
    /*!
     \brief Get thread priority

     \return int
    */
    int GetPriority(void);
    /*!
     \brief Speed up thread execution - increase priority level

    */
    void SpeedUp(void);
    /*!
     \brief Slow down Thread execution - decrease priority level

    */
    void SlowDown(void);
    /*!
     \brief Terminate immediate the thread Unsafe

    */
    void Terminate(void);

protected:
    /*!
     \brief Thread function

     \param lpParameter Add param
     \return unsigned __stdcall
    */
    static unsigned __stdcall _ThreadProc(LPVOID lpParameter);
    /*!
     \brief Exit the thread safety

    */
    void Exit(void);
    /*!
     \brief Call the running member function

    */
    inline void OnRunning(void);

protected:
    T& m_thObject;               /*!< Add in-line comment */
    void (T::*m_pfnOnRunning)(); /*!< Add in-line comment */
    HANDLE m_hThread,            /*!< Thread handle */
           m_hEvent;             /*!< Event handle */
    int m_nInitPriority;         /*!< Add in-line comment */
    unsigned int m_dwThreadID;   /*!< Contain the thread ID */
    bool m_bTerminate,           /*!< Thread state is terminated */
         m_bSuspend,             /*!< Thread state is suspended */
         m_bIsRunning;           /*!< Thread state is running */
};

//-- End of TThread Class Declaration --

//-- Start of TThread Definition --

/*!
 \brief TThread constructor

 \param thObject Add param
 \param (*pfnOnRunning)() Add param
 \param nPriority Add param
*/
template<typename T> TThread<T>::TThread(T& thObject,void (T::*pfnOnRunning)(), int nPriority):m_thObject(thObject),m_pfnOnRunning(pfnOnRunning)
{
    m_hThread = NULL;
    m_dwThreadID = 0;
    m_bTerminate = true;
    m_bSuspend = true;
    m_bIsRunning = false;
    m_nInitPriority = nPriority;
    m_hEvent = ::CreateEvent(NULL,FALSE,FALSE,NULL);
}

/*!
 \brief Start

 \return bool
*/
template<typename T> bool TThread<T>::Start(void)
{
    if(m_bTerminate)
    {
        m_hThread = (HANDLE)_beginthreadex(NULL,0,_ThreadProc,(LPVOID)this,0/* CREATE_SUSPENDED*/,&m_dwThreadID);

        m_bTerminate = false;
        m_bSuspend = false;

        if(m_hThread != 0)
            return true;
        return false;
    }
    else if(m_bSuspend)
    {
        DWORD nRet = ::ResumeThread(m_hThread);
        if(nRet == 0xFFFFFFFF)
            return false;
        m_bSuspend = false;
        return true;
    }
    return true;
}

/*!
 \brief StartAndWait

 \return bool
*/
template<typename T> bool TThread<T>::StartAndWait(void)
{
    bool bRet = Start();
    if(bRet)
        ::WaitForSingleObject(m_hEvent,INFINITE);
    return bRet;
}

/*!
 \brief Pause

 \return bool
*/
template<typename T> bool TThread<T>::Pause(void)
{
    if(m_bTerminate)
        return false;
    if(m_bSuspend)
        return true;
    DWORD nRet = ::SuspendThread(m_hThread);
    if(nRet == 0xFFFFFFFF)
        return false;
    m_bSuspend = true;
    return true;
}

/*!
 \brief IsRunning

 \return bool
*/
template<typename T> bool TThread<T>::IsRunning(void)
{
    return m_bIsRunning;
}

/*!
 \brief IsTerminated

 \return bool
*/
template<typename T> bool TThread<T>::IsTerminated(void)
{
    return m_bTerminate;
}

/*!
 \brief IsSuspend

 \return bool
*/
template<typename T> bool TThread<T>::IsSuspend(void)
{
    return m_bSuspend;
}

/*!
 \brief Terminate

*/
template<typename T> void TThread<T>::Terminate(void)
{
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
        ::TerminateThread(m_hThread,dwExitCode);
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
}

/*!
 \brief Exit

*/
template<typename T> void TThread<T>::Exit(void)
{
    DWORD dwExitCode;
    ::GetExitCodeThread(m_hThread,&dwExitCode);
    if (STILL_ACTIVE == dwExitCode)
    {
        _endthreadex(dwExitCode);
        ::ExitThread(dwExitCode);
        ::CloseHandle(m_hThread);
        m_hThread = NULL;
    }
    m_bIsRunning = false;
    m_bTerminate = true;
}

/*!
 \brief WaitUntilTerminate

 \param dwMiliSec Add param
 \return bool
*/
template<typename T> bool TThread<T>::WaitUntilTerminate(DWORD dwMiliSec)
{
    if(WaitForSingleObject(m_hThread,dwMiliSec) == WAIT_TIMEOUT)
        return false;
    m_bIsRunning = false;
    m_bTerminate = true;
    return true;
}

/*!
 \brief SetPriority

 \param nLevel Add param
*/
template<typename T> void TThread<T>::SetPriority(int nLevel)
{
    ::SetThreadPriority(m_hThread,nLevel);
}

/*!
 \brief GetPriority

 \return int
*/
template<typename T> int TThread<T>::GetPriority(void)
{
    return ::GetThreadPriority(m_hThread);
}

/*!
 \brief SpeedUp

*/
template<typename T> void TThread<T>::SpeedUp(void)
{
    int nOldLevel;
    int nNewLevel;

    nOldLevel = GetPriority();
    if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
        return;
    else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_TIME_CRITICAL;
    else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_HIGHEST;
    else if (THREAD_PRIORITY_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_NORMAL;
    else if (THREAD_PRIORITY_LOWEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
    else if (THREAD_PRIORITY_IDLE == nOldLevel)
        nNewLevel = THREAD_PRIORITY_LOWEST;

    SetPriority(nNewLevel);
}

/*!
 \brief SlowDown

*/
template<typename T> void TThread<T>::SlowDown(void)
{
    int nOldLevel;
    int nNewLevel;

    nOldLevel = GetPriority();
    if (THREAD_PRIORITY_TIME_CRITICAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_HIGHEST;
    else if (THREAD_PRIORITY_HIGHEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_ABOVE_NORMAL;
    else if (THREAD_PRIORITY_ABOVE_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_NORMAL;
    else if (THREAD_PRIORITY_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_BELOW_NORMAL;
    else if (THREAD_PRIORITY_BELOW_NORMAL == nOldLevel)
        nNewLevel = THREAD_PRIORITY_LOWEST;
    else if (THREAD_PRIORITY_LOWEST == nOldLevel)
        nNewLevel = THREAD_PRIORITY_IDLE;
    else if (THREAD_PRIORITY_IDLE == nOldLevel)
        return;

    SetPriority(nNewLevel);
}

/*!
 \brief OnRunning

*/
template<typename T> void TThread<T>::OnRunning(void)
{
    (m_thObject.*m_pfnOnRunning)();
}

/*!
 \brief

 \param lpParameter Add param
 \return unsigned __stdcall
*/
template<typename T> unsigned __stdcall TThread<T>::_ThreadProc(LPVOID lpParameter)
{
    TThread<T>* pThread = reinterpret_cast<TThread<T>*>(lpParameter);

    pThread->SetPriority(pThread->m_nInitPriority);
    pThread->m_bIsRunning = true;
    ::SetEvent(pThread->m_hEvent);
    pThread->OnRunning();
    return 0;
}
