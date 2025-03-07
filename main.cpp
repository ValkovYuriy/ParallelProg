#include "tbag.h"
#include <stdio.h>

#include <iostream>
using namespace std;

// ������������ ��������� ������
// � �������������� �������� �����

const int P=10;//����� ������� ��������� (�� ������������ � MPI)
const int N=10;
double a[N][N],b[N][N],c[N][N];

class TaskBag:public TEMPLET::TBag{
public:
    class TaskBagTask:public TBag::Task{
    public:
        TaskBagTask():TBag::Task(){}
        virtual~TaskBagTask(){}

        void send_task()  {send(&num,sizeof(num));}// ������ ������ � �����  (������ ��� �������������� ����������)
        void recv_task()  {recv(&num,sizeof(num));}// ������ ������ �� ������ (������ ��� �������������� ����������)
        void send_result(){send(&num,sizeof(num)); send(c[num],sizeof(double)*N);}// ������ ���������� � ����� (������ ��� �������������� ����������)
        void recv_result(){recv(&num,sizeof(num)); recv(c[num],sizeof(double)*N);}// ������ ���������� �� ������ (������ ��� �������������� ����������)

        int num;// ����� ����������� ������
    };
public:
    TaskBag(int num_prc,int argc, char* argv[]):TBag(num_prc,argc,argv){
        cur=0;//����� ������� ������ � ������� �
    }
    virtual ~TaskBag(){}
    TBag::Task* createTask(){return new TaskBagTask;}

    bool if_job(){return cur<N;}
    void put(Task*t){TaskBagTask* mt=(TaskBagTask*)t; }
    void get(Task*t){TaskBagTask* mt=(TaskBagTask*)t; mt->num=cur++;}
    void proc(Task*t){TaskBagTask* mt=(TaskBagTask*)t;
        int i=mt->num;
        for(int j=0;j<N;j++){// ������������ ���������� ������ ������� C
            c[i][j]=0.0;
            for(int k=0;k<N;k++)c[i][j]+=a[i][k]*b[k][j];
        }
    }

    int cur;//����� ������� ������ � ������� �
};

int main(int argc, char* argv[])
{

    TaskBag bag(P,argc,argv);//���������� P ������� ���������(�� ������������ � MPI)
    // �������������
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)a[i][j]=N*i+j;

    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)b[i][j]=N*i+j;

    // ������������ ��������� ������
    bag.run();
    cout<<"\nspeedup = "<<bag.speedup();//1-��� ���������� �������,
    //P-��� ������������ ���������,
    //��������������� �������� ��������� ��� ������� P ��� ���������-���������� �������� ����������
    //��������! ����� �������� �������� ������ ��� �� Windows

    cout<<"\nduration = "<<bag.duration()<< " sec\n";

    // ����� ���������� ������������� ���������
    // ��������� ��� ������� N
    cout<<"\nC(parallel)=\n";
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            cout<<c[i][j]<<" ";
        }
        cout<<'\n';
    }

    // ������� ������� �
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++) c[i][j]=0.0;

    // ���������������� ��������� ������
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            c[i][j]=0.0;
            for(int k=0;k<N;k++)c[i][j]+=a[i][k]*b[k][j];
        }
    }

    // ����� ���������� ����������������� ���������
    // ��������� ��� ������� N
    cout<<"\nC(serial)=\n";
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++){
            cout<<c[i][j]<<" ";
        }
        cout<<'\n';
    }
    return 0;
}