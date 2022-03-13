#include "asio.hpp"
#include "control.hpp"
#include "logic.hpp"
// #include "player.hpp"
using namespace std;
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13

void print_card(CARD::Card c){
    char chr=' ';
    if(c.num==1)chr='A';
    else if(c.num==11)chr='J';
    else if(c.num==12)chr='Q';
    else if(c.num==13)chr='K';
    else if(c.num==14)chr='S';
    else if(c.num==15)chr='B';
    else if(c.num==10){
        cout<<tab[c.flo]<<"10";
        return;
    }
    else chr=c.num+'0';
    if(c.flo==1||c.flo==2||c.num==15)sred();
    else sblack();
    cout<<tab[c.flo]<<chr<<" ";
}
// using namespace CARD;
namespace Game
{
    CARD::Card heap[55]; 
    // Player pl[3];
    string username[3];
    int plc[3]; //player left card
    int lord,nowturn;
    CARD::out_card string_to_outcard(string s){
        CARD::out_card res;
        vector<CARD::Card> cards;cards.clear();
        for(int i=0,now=0,flo=0,num=0;i<s.length();i++){
            if(s[i]==';'){
                cards.emplace_back(flo,num);
                flo=num=now=0;
            }else if(s[i]==','){
                now++;
            }else{
                if(now==0)flo=flo*10+s[i]-'0';
                else num=num*10+s[i]-'0';
            }
        }
        res.get_card(cards);
        return res;
    }
    string outcard_to_string(CARD::out_card x){
        string s="";
        for (int i=1; i<=x.t; i++) s+=to_string(x.A[i].flo)+","+to_string(x.A[i].num)+";";
        return s;
    }
    namespace Network{
        asio::io_context ioc;
        array<char,65536> buf;
        asio::error_code err;
        void send(asio::ip::tcp::socket *sock, string msg){
            asio::write(*sock,asio::buffer(msg));
        }
        string read(asio::ip::tcp::socket *sock){
            size_t len=(*sock).read_some(asio::buffer(buf));
            string res=buf.data();
            res.resize(len);
            return res;
        }
        
        CARD::out_card card_getting(asio::ip::tcp::socket *sock)
        {
            string s=read(sock);
            // cards.push_back(CARD::Card());
            if (s[0]=='@') return CARD::out_card();
            return string_to_outcard(s);
        }
        void card_sending(asio::ip::tcp::socket *sock, CARD::out_card x)
        {
            Network::send(sock, outcard_to_string(x));
        }
    }
    namespace Client{
        //hand cards
        vector<CARD::Card> hcard;
        CARD::out_card lord_cards;
        asio::ip::tcp::socket sock(Network::ioc); 
        int myturn; //my turn-id: increase each time the game end
        CARD::out_card lastout;
        int lastoutplayer;
        vector<CARD::Card> card_chosen;
        void data_init(){
            hcard.clear();
            // myturn=0;
            card_chosen.clear();
            plc[0]=plc[1]=plc[2]=17;
        }
        void read_predata(){
            myturn=Network::read(&sock)[0]-'0';     //get my turn
            // string re=Network::read(&sock);         //read a number meaning game starting
            for (int i=0; i<3; i++) username[i]=Network::read(&sock);   //get players' names
            // cls();
        }
        void start_client(){
            cout<<"告诉爷，你叫啥玩意：";
            string name;cin>>name;
            while(name.length()>30) {
                cout<<"做男♂人不能太长♂ ——czq"<<endl;
                cin>>name;
            }
            bool connect_ok=false;
            while(!connect_ok){
                cout<<"给爷IP：";
                string ipaddr;cin>>ipaddr;
                try{
                    asio::connect(sock,asio::ip::tcp::resolver(Network::ioc).resolve(ipaddr,std::to_string(12345)));
                    connect_ok=true;
                }catch(...){
                    cout<<"你是不是脑抽了？\n";
                }
            }
            Network::send(&sock,name);
            cout<<"小东西居然让你连上了\n";
            
        }
        void print_hcard(){
            sort(hcard.begin(),hcard.end());
            gotoxy(10,0);
            for(auto c:hcard)print_card(c),putchar(' ');
        }
        void print_out_card(CARD::out_card x)
        {
            gotoxy(4, 0);
            for (int i=1; i<=x.t; i++) print_card(x.A[i]), putchar(' ');
        }
        void print_left_cards(int player)
        {
            if (nowturn==player) sred();
            else sblack();
            cout<<username[player];
            if (lord==player) cout<<"（地主）";
            cout<<"："<<plc[player]<<"张";
            sblack();
        }
        void print_lord_card(){
            gotoxy(0,0);
            sblack();
            cout<<"地主牌：";
            print_card(lord_cards.A[1]),putchar(' ');
            print_card(lord_cards.A[2]),putchar(' ');
            print_card(lord_cards.A[3]),putchar(' ');
        }
        void repaint(){
            cls();
            print_lord_card();
            for (int i=0, sb=0; i<=2; i++) if (i!=myturn) gotoxy(++sb, 0), print_left_cards(i);
            gotoxy(3, 0);
            if (lastout.t)
            {
                if (nowturn==myturn) sred();
                else sblack();
                cout<<username[lastoutplayer]<<(lastoutplayer==myturn?"（你）：":"：")<<endl;
                sblack();
                print_out_card(lastout);
            }
            if (nowturn==myturn) sred();
            else sblack();
            gotoxy(5, 0); cout<<username[myturn]<<"（你）:";
            sblack();
            print_hcard();
        }
        void get_hand_card()
        {
            hcard.clear();
            CARD::out_card cards=Network::card_getting(&sock);  //read hand cards
            for (int i=1; i<=cards.t; i++) hcard.push_back(cards.A[i]); 
        }
        void get_lord_card(){
            CARD::out_card cards=Network::card_getting(&sock);  //read lord cards
            lord_cards=cards;
            print_lord_card();
            if(lord==myturn){
                hcard.push_back(lord_cards.A[1]);
                hcard.push_back(lord_cards.A[2]);
                hcard.push_back(lord_cards.A[3]);    
            }
            plc[lord]+=3;
        }
        void get_lord()
        {
            int ok=0, isget[3]={0};
            for (int turn=0; turn<=2; turn++)
            {
                // cls();
                gotoxy(0,0);for(int i=0;i<6;i++)cout<<"                         "<<endl;gotoxy(0,0);
                for (int i=0; i<3; i++) if (i!=myturn)
                {
                    if (turn==i) sred();
                    std::cout<<username[i]<<":"<<(isget[i]?(isget[i]==2?"叫地主":"不叫"):"")<<endl;
                    sblack();
                }
                if (turn==myturn) sred();
                std::cout<<username[myturn]<<"（你）:"<<(isget[myturn]?(isget[myturn]==2?"叫地主":"不叫"):"")<<endl;
                sblack();
                if (turn==myturn)
                {
                    cout<<"是否叫地主（y/n）";
                    while (1)
                    {
                        char x=_getch();
                        if (x=='y') {Network::send(&sock, to_string(1)); break;}
                        if (x=='n') {Network::send(&sock, to_string(0)); break;}
                    }
                }
                int re=Network::read(&sock)[0]-'0';
                isget[turn]=re+1, ok+=re;
            }
            if (!ok) return cls(), get_hand_card(), print_hcard(), get_lord(); //nobody lord, again
            if (ok==1) return (void)(lord=isget[0]==2?0:(isget[1]==2?1:2));   //only one lord, get_lord end
            for (int turn=0; turn<=2; turn++) if (isget[turn]!=1)  //many players lord, re-lord
            {
                // cls();
                gotoxy(0,0);for(int i=0;i<6;i++)cout<<"                         "<<endl;gotoxy(0,0);
                //output label
                for (int i=0; i<3; i++) if (i!=myturn)
                {
                    if (turn==i) sred();
                    std::cout<<username[i]<<":"<<(isget[i]?(isget[i]==2?"叫地主":"不叫"):"")<<(i<turn?" 不抢":"")<<endl;
                    sblack();
                }
                if (turn==myturn) sred();
                std::cout<<username[myturn]<<"（你）:"<<(isget[myturn]?(isget[myturn]==2?"叫地主":"不叫"):"")<<(myturn<turn?" 不抢":"")<<endl;;
                sblack();

                // operating area
                if (turn==myturn)
                {
                    cout<<"是否抢地主（y/n）";
                    while (1)
                    {
                        char x=_getch();
                        if (x=='y') {Network::send(&sock, to_string(1)); break;}
                        if (x=='n') {Network::send(&sock, to_string(0)); break;}
                        //not 'Y' and 'N', or you will press shift
                    }
                }
                int re=Network::read(&sock)[0]-'0';
                if (re) return (void)(lord=turn);
            }
            return (void)(lord=isget[0]==2?0:(isget[1]==2?1:2));
        }
        void use_card(){
            int ptr=0,len=hcard.size();
            bool chosen[25]={0};
            sblack(),gotoxy(8,0);cout<<"按 Enter 出牌/跳过";
            auto clear_illegal=[]()->void{
                gotoxy(7, 0), sblack(), cout<<"                         ";
            };
            sred();
            gotoxy(11,2+ptr*5);cout<<"^";
            bool confirm=0;
            while(1){
                sblack(),gotoxy(8,0);cout<<"按两次 Enter 出牌/跳过";
                char c=_getch();
                if(c==KEY_ENTER){
                    if(!confirm){confirm=1;continue;}
                    // Network::send(&sock,"@");
                    card_chosen.clear();
                    for(int i=0; i<len; i++) if (chosen[i]) card_chosen.push_back(hcard[i]);
                    
                    
                    CARD::out_card temp;
                    temp.get_card(card_chosen);
                    temp.identify();
                    if ((temp.k&&(lastoutplayer!=myturn
                        &&(temp.k==lastout.k&&temp.t==lastout.t||temp.k==6||temp.k==12)
                        &&temp.sym>lastout.sym
                        ||lastoutplayer==myturn&&temp.t))
                        ||lastoutplayer!=myturn&&!temp.t)
                    {
                    
                        if(!temp.t) {
                            Network::send(&sock,"@");
                            return;
                        }
                        lastout=temp; int c=0;
                        for (int i=0; i<len; i++) if (chosen[i]) hcard.erase(hcard.begin()+i-c), c++;
                        plc[myturn]-=c;
                        lastoutplayer=myturn;
                        Network::card_sending(&sock,lastout);
                        return;
                    }
                    else gotoxy(7, 0), sblack(), cout<<"出牌不符合游戏规则";
                    
                }
                else if(c==' '){
                    clear_illegal();
                    chosen[ptr]^=1;
                    
                    sblack();
                    gotoxy(10,ptr*5);cout<<"     ";
                    gotoxy(9,ptr*5);cout<<"     ";
                    gotoxy(10-chosen[ptr],ptr*5);
                    print_card(hcard[ptr]);
                    confirm=0;
                }
                else if(c==KEY_LEFT&&ptr>0){
                    clear_illegal();
                    sred();
                    gotoxy(11,2+ptr*5);cout<<" ";
                    ptr--;
                    gotoxy(11,2+ptr*5);cout<<"^";
                    confirm=0;
                }
                else if(c==KEY_RIGHT&&ptr<len-1){
                    clear_illegal();
                    sred();
                    gotoxy(11,2+ptr*5);cout<<" ";
                    ptr++;
                    gotoxy(11,2+ptr*5);cout<<"^";
                    confirm=0;
                }
            }
        }
    }
    namespace Server{
        asio::ip::tcp::acceptor ac(Network::ioc);
        struct Sock{asio::ip::tcp::socket sock; Sock():sock(Network::ioc) {}}client[3];
        void listen_client();
        void send_predata(){
            Sleep(1000);     //if no sleep, the last player could not receive the message
            for (int p=0; p<3; p++) Network::send(&client[p].sock, to_string(p));
            Sleep(100);
            for (int p=0; p<3; p++)
                for (int i=0; i<3; i++)
                    Network::send(&client[p].sock, username[i]),Sleep(50);
        }
        void start_server(){
            ac=asio::ip::tcp::acceptor(Network::ioc,asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 12345));
            listen_client();
        }
        void listen_client(){
            for(int p=0; p<3; p++){
                client[p].sock=ac.accept();
                username[p]=Network::read(&client[p].sock);
                cout<<"有一个SB前来连接："<<username[p]<<endl;
            }
        }
        void send_hand_card()
        {
            srand(time(NULL));
            for (int i=1; i<=52; i++) heap[i]={(i-1)/13+1, (i-1)%13+1};
            heap[53]={0, 14}; heap[54]={0, 15};
            random_shuffle(heap+1, heap+55);    //shuffle the cards
            // for (int i=1; i<=54; i++) print_card(heap[i]), putchar(' ');
            // putchar('\n');putchar('\n');
            for (int p=0; p<3; p++){
                CARD::out_card cards;
                for (int i=1 + 17*p; i<=17 + 17*p; i++) cards.A[++cards.t]=heap[i];
                Network::card_sending(&Server::client[p].sock,cards);   //send players' cards
            }
            // 1-17: pl1, 18-34: pl2, 35-51: pl3, 52-54: lord_card

        }
        void send_lord_card(){
            CARD::out_card cards;
            for (int i=52; i<=54; i++) cards.A[++cards.t]=heap[i];
            for (int p=0; p<3; p++) Network::card_sending(&Server::client[p].sock,cards);   //send lord cards
        }
        
        void get_lord()
        {
            int ok=0, isget[3];
            for (int turn=0; turn<=2; turn++)
            {
                isget[turn]=Network::read(&client[turn].sock)[0]-'0'; ok+=isget[turn];
                Sleep(50);
                for (int i=0; i<=2; i++) Network::send(&client[i].sock, to_string(isget[turn]));
            }
            if (!ok) return send_hand_card(), get_lord();
            if (ok==1) return (void)(lord=isget[0]?0:(isget[1]?1:2));
            for (int turn=0; turn<=2; turn++) if (isget[turn])
            {
                int re=Network::read(&client[turn].sock)[0]-'0';
                Sleep(50);
                for (int i=0; i<=2; i++) Network::send(&client[i].sock, to_string(re));
                if (re) return (void)(lord=turn);
            }
            return (void)(lord=isget[0]?0:(isget[1]?1:2));
        }
    }
    
}
int main()
{
    cp();
    // Game::send_card();
    cout<<"斗主地：客户端=1，服务端=2：";
    // cout<<"Rubbish rubbish wawasisi";
    char c=getchar();
    while(c!='1'&&c!='2')printf("sbabs\n"),c=getchar();
    if(c=='1'){
        cls();
        // printf("Client start");
        Game::Client::start_client();
        while(1)    //game loop
        {
            //prework
            
            Game::Client::data_init();
            Game::Client::read_predata();
            Game::Client::get_hand_card();
            cls();
            Game::Client::print_hcard();
            Game::Client::get_lord();

            Game::nowturn=Game::lord;
            Game::Client::lastoutplayer=Game::lord;

            Game::Client::get_lord_card();
            while(1){
                // SetConsoleTitle((Game::Client::lastoutplayer==0?L"0":Game::Client::lastoutplayer==1?L"1":L"2"));
                Game::Client::repaint();
                if(Game::Client::lastoutplayer==Game::nowturn){
                    CARD::out_card oc;
                    Game::Client::lastout=oc;
                    Game::Client::repaint();
                }
                if (Game::nowturn==Game::Client::myturn) {
                    Game::Client::use_card();
                }
                else
                {
                    CARD::out_card temp=Game::Network::card_getting(&Game::Client::sock);
                    if (temp.t)
                    {
                        Game::Client::lastout=temp;
                        Game::Client::lastout.identify();
                        Game::Client::lastoutplayer=Game::nowturn;
                        Game::plc[Game::nowturn]-=temp.t;
                    }
                }
                if(Game::plc[Game::nowturn]==0){
                    Game::Client::repaint();
                    Sleep(2000);
                    cls();
                    sred();
                    for(int i=1;i<=250;i++)cout<<(Game::nowturn==Game::lord?"地主获胜！":"农民获胜！");
                    Sleep(2000);
                    break;
                }
                Game::nowturn=(Game::nowturn+1)%3;
            }
            cls();
        }
    }
    if(c=='2'){
        cls();
        cout<<"伞兵一号卢本伟准备就绪\n";
        Game::Server::start_server();
        while(1)    //game loop
        {
            Game::plc[0]=Game::plc[1]=Game::plc[2]=17;
            Game::Server::send_predata();
            Game::Server::send_hand_card();
            Game::Server::get_lord();
            Sleep(1000); //delay
            Game::nowturn=Game::lord;
            Game::plc[Game::lord]+=3;
            Game::Server::send_lord_card();
            // Game::Network::send(&Game::Server::client[Game::lord].sock,"@");
            while(1){
                string re=Game::Network::read(&Game::Server::client[Game::nowturn].sock);
                if(re=="@"){}
                else{
                    Game::plc[Game::nowturn]-=Game::string_to_outcard(re).t;
                }
                for(int p=0; p<3; p++) if(p!=Game::nowturn) Game::Network::send(&Game::Server::client[p].sock,re);
                if(Game::plc[Game::nowturn]==0){
                    Sleep(6000);
                    
                    string tmp=Game::username[2];
                    Game::username[2]=Game::username[1];
                    Game::username[1]=Game::username[0];
                    Game::username[0]=tmp;

                    asio::ip::tcp::socket tmpsock=move(Game::Server::client[2].sock);
                    Game::Server::client[2].sock=move(Game::Server::client[1].sock);
                    Game::Server::client[1].sock=move(Game::Server::client[0].sock);
                    Game::Server::client[0].sock=move(tmpsock);
                    Sleep(500);
                    break;
                }

                Game::nowturn=(Game::nowturn+1)%3;
            }
        }
        // Game::Server::get_lord();
    }

    while(1);
}