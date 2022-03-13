#include "logic.hpp"

namespace CARD{
    const int cmp[16]={0, 12, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15};
    bool CARD::Card::operator < (const Card &x) const{return cmp[num]!=cmp[x.num]?cmp[num]<cmp[x.num]:flo<x.flo;}
    void CARD::out_card::get_card(vector<CARD::Card> cs){
        for(auto i:cs){
            A[++t]=i;
        }
    }
    void CARD::out_card::identify()
    {
        sort(A+1, A+t+1); int a[25];
        for (int i=1; i<=t; i++) a[i]=A[i].num;
        if (t==1) return (void)(k=1, sym=cmp[a[1]]);//单张
        if (t==2&&a[1]==a[2]) return (void)(k=2, sym=cmp[a[1]]);//对子
        if (t==2&&a[1]==14&&a[2]==15) return (void)(k=12, sym=1919810);//王炸
        if (t==3&&a[1]==a[3]) return (void)(k=3, sym=cmp[a[1]]);//三张
        if (t==4)
        {
            if (a[1]==a[4]) return (void)(k=6, sym=cmp[a[1]]+114514);//炸弹
            if (a[1]==a[3]) return (void)(k=4, sym=cmp[a[1]]);
            if (a[2]==a[4]) return (void)(k=4, sym=cmp[a[2]]);//三带一
        }
        if (t>=5)
        {
            if (t==5)
            {
                if (a[1]==a[3]&&a[4]==a[5]) return (void)(k=5, sym=cmp[a[1]]);
                if (a[3]==a[5]&&a[1]==a[2]) return (void)(k=5, sym=cmp[a[3]]);
            }//三带二
            if (t==6)
            {
                if (a[1]==a[4]) return (void)(k=11, sym=cmp[a[1]]);
                if (a[2]==a[5]) return (void)(k=11, sym=cmp[a[2]]);
                if (a[3]==a[6]) return (void)(k=11, sym=cmp[a[3]]);
            }
            if (t==8)
            {
                if (a[1]==a[4]&&a[5]==a[6]&&a[7]==a[8]) return (void)(k=11, sym=cmp[a[1]]);
                if (a[1]==a[2]&&a[3]==a[6]&&a[7]==a[8]) return (void)(k=11, sym=cmp[a[2]]);
                if (a[1]==a[2]&&a[3]==a[4]&&a[5]==a[8]) return (void)(k=11, sym=cmp[a[3]]);
            }//四带二
            int ok=1;
            for (int i=2; i<=t; i++)
                if (cmp[a[i]]!=cmp[a[i-1]]+1) {ok=0; break;} 
            if (ok&&cmp[a[t]]<=12) return (void)(k=7, sym=cmp[a[1]]);//顺子
            ok=1;
            if (!(t&1)&&a[1]==a[2])
            {
                for (int i=3; i<=t; i+=2) if (a[i]!=a[i+1]||cmp[a[i]]!=cmp[a[i-1]]+1) {ok=0; break;}
                if (ok&&cmp[a[t]]<=12) return (void)(k=8, sym=cmp[a[1]]);//连对
            }
            ok=1;
            if (t%3==0&&a[1]==a[3])
            {
                for (int i=4; i<=t; i+=3) if (a[i]!=a[i+2]||cmp[a[i]]!=cmp[a[i-1]]+1) {ok=0; break;}
                if (ok&&cmp[a[t]]<=12) return (void)(k=9, sym=cmp[a[1]]);//三顺子
            }
            if (t%4==0)
            {
                int cnt[20]={0};
                for (int i=1; i<=t; i++) cnt[cmp[a[i]]]++;
                int be=0, en=0;
                for (int i=1; i<=12; i++) if (cnt[i]>=3) {be=i; break;}
                if (be)
                {
                    for (int i=be; i<=13; i++) if (i==13||cnt[i]<3) {en=i-1; break;}
                    if ((en-be+1)*4==t) return (void)(k=10, sym=cmp[a[be]]);
                }
            }//飞机带单翅膀
            if (t%5==0)
            {
                int cnt[20]={0};
                for (int i=1; i<=t; i++) cnt[cmp[a[i]]]++;
                int be=0, en=0, c_2=0;
                for (int i=1; i<=12; i++)
                {
                    if (cnt[i]==3&&!be) be=i;
                    if (cnt[i]==2) c_2++;
                    if (cnt[i]==4) c_2+=2;
                }
                if (!be) return (void)(k=0);
                for (int i=be; i<=13; i++) if (i==13||cnt[i]!=3) {en=i-1; break;}
                if (en-be+1==c_2) return (void)(k=13, sym=cmp[a[be]]);
            }//飞机带双翅膀
            return (void)(k=0);
        }
}
}