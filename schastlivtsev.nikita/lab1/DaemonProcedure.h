#ifndef DAEMONPROCEDURE_H
#define DAEMONPROCEDURE_H


class DaemonProcedure
{
    public:
        DaemonProcedure();
        virtual ~DaemonProcedure();
        virtual void run() = 0;
    protected:
    private:
};

#endif // DAEMONPROCEDURE_H
