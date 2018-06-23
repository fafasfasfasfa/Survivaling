#include "gracz.h"
#include "structy.h"
#include "swiat.h"
#include <windows.h>



void czesc_ciala::zadaj_obrazenia(short ile)
{
         if(hp-ile<0) hp=0;
         else hp-=ile;


}

void czesc_ciala::minela_runda(warunki war)
{

}

void czesc_ciala::ulecz(short ile)
{
         if(hp+ile<maxx) hp+=ile; else hp=maxx;
}


Gracz :: Gracz():Objekt_zywy(10), klatka(40, 40), brzuch(30, 30), ramie_l(10, 10), ramie_p(10, 10), l_dlon(5, 5), p_dlon(5, 5)
                  , l_udo(15, 15), p_udo(15, 15), l_golen(10, 10), p_golen(10, 10)
{
         wykonuje_ruch=false;
         wysokosc=1; if(postawa=='r') wysokosc=3; else if(postawa=='s') wysokosc=2;
}

void Gracz::ogrzej_sie_od_zrodel_ognia()
{
         Plansza *p = swiat->zwroc_taka_plansze_TYLKO(px, py, pz);
         for(short f=x-1; f<=x+1; ++f)
         {
                  for(short g=y-1; g<=y+1; ++g)
                  {
                           if(Strefa::w_planszy(f, g))
                           {
                                    if(p->srodowisko[g][f]!=NULL)
                                    {
                                             if((p->srodowisko[g][f]->stan&1)==1)
                                                      dodaj_cieplo(10);
                                             if(p->srodowisko[g][f]->wierzch() && p->srodowisko[g][f]->wierzch()->czym_jest==24 && ((Ognisko*)p->srodowisko[g][f]->wierzch())->getPaliSie())
                                                      dodaj_cieplo(10);
                                    }
                                    if(p->otoczenie[g][f]!=NULL && (p->otoczenie[g][f]->stan&1)==1)
                                             dodaj_cieplo(10);
                           }
                  }
         }
         if(p->srodowisko[y][x]!=NULL && (p->srodowisko[y][x]->stan&1)==1)
                  dodaj_cieplo(40);
}

bool Gracz::zaktualizuj_po_rundzie()///jesli martwy to zwroc false
{
         if(p_rece!=NULL && p_rece->czym_jest==2201) if(p_rece->wykorzystaj()) {Item*h=p_rece; p_rece=new Konsumpcjum(2202, 0); delete h;}
         if(p_kieszenl!=NULL && p_kieszenl->czym_jest==2201) if(p_kieszenl->wykorzystaj()) {Item*h=p_kieszenl; p_kieszenl=new Konsumpcjum(2202, 0); delete h;}
         if(p_kieszenp!=NULL && p_kieszenp->czym_jest==2201) if(p_kieszenp->wykorzystaj()) {Item*h=p_kieszenp; p_kieszenp=new Konsumpcjum(2202, 0); delete h;}

         if(*Objekt::args->godmode) return true;

         short pogoda = swiat->pogoda;
         if(pozostalo_snu>1) return true;

         ogrzej_sie_od_zrodel_ognia();

         if(woda_a>0) --woda_a; else {if(!odejmij_hp_z_powodu_niedozywienia()){return false;}else if(!odejmij_hp_z_powodu_niedozywienia()){return false;};}
         if(jedzenie_a>0) --jedzenie_a; else {if(!odejmij_hp_z_powodu_niedozywienia()){return false;}else if(!odejmij_hp_z_powodu_niedozywienia()){return false;};}
         if(energia_a>0) --energia_a;
         if(ocieplenie()<pogoda*10) {if(cieplo_a>0){--cieplo_a;} else if(!odejmij_hp_z_powodu_niedozywienia()){return false;}}else if(cieplo_a<cieplo_max)++cieplo_a;

         //oszolomieni.wywal_wszystkie_objekty();
         int ile_zal_kond=(woda_a>0)+p_ruchu/5;
         if(kondycja==0 && ile_zal_kond==0) ile_zal_kond+=1;
         if(kondycja+ile_zal_kond>18) kondycja=18;
         else kondycja+=ile_zal_kond;
         p_ruchu=10;

         warunki war = zle;
         if(pozostalo_snu==1) return true; ///zeby nie bylo oszukiwania podczas przetrzymywania

         //if(jedzenie_a>150 && woda_a>150 && cieplo_a>150) {if(leczenie<40)++leczenie;} else if(leczenie>0) --leczenie;
         if(jedzenie_a>150 && woda_a>150 && cieplo_a>150 && energia_a>100) war = bardzo_dobre;
         else if(jedzenie_a>100 && woda_a>100 && cieplo_a>100 && energia_a>100) war = dobre;
         else
         {
                  short licznik = (jedzenie_a>70) + (woda_a>70) + (cieplo_a>80) + (energia_a>60);
                  if(licznik >= 3) war = srednie;
         }

         klatka.minela_runda(war);
         brzuch.minela_runda(war);
         ramie_l.minela_runda(war);
         ramie_p.minela_runda(war);
         l_dlon.minela_runda(war);
         p_dlon.minela_runda(war);
         l_udo.minela_runda(war);
         p_udo.minela_runda(war);
         l_golen.minela_runda(war);
         p_golen.minela_runda(war);

         return czy_zyje();

         return true;
}

bool Gracz::gracz_widzi_do_konca_w_strone(char strona)
{
         Gracz*g=swiat->aktualny;Plansza*p=swiat->aktualna;
         switch(strona)
         {
                  case 'p':if(g->x+g->zasieg_wzroku()<21 || swiat->zwroc_taka_plansze(px+1, py, pz)==NULL){return false;}break;
                  case 'l':if(g->x-g->zasieg_wzroku()>-1 || swiat->zwroc_taka_plansze(px-1, py, pz)==NULL)return false;break;
                  case 'g':if(g->y-g->zasieg_wzroku()>-1 || swiat->zwroc_taka_plansze(px, py-1, pz)==NULL)return false;break;
                  case 'd':if(g->y+g->zasieg_wzroku()<13 || swiat->zwroc_taka_plansze(px, py+1, pz)==NULL)return false;break;
         }
         if(strona=='p' || strona=='l'){
                  int start=g->x;if(strona=='p') ++start; else --start;
                  while((strona=='p'&&start<21)||(strona=='l'&&start>-1)){
                           if(p->otoczenie[g->y][start]!=NULL && p->otoczenie[g->y][start]->wysokosc>=g->wysokosc) {return false;}
                           if(strona=='p') ++start; else --start;
                  }
                  if(strona=='p' && swiat->zwroc_taka_plansze(g->px+1,g->py,g->pz) && swiat->zwroc_taka_plansze(g->px+1,g->py,g->pz)->otoczenie[g->y][0]!=NULL&&swiat->zwroc_taka_plansze(g->px+1,g->py,g->pz)->otoczenie[g->y][0]->wysokosc>=g->wysokosc){return false;}
                  else if(strona=='l' && swiat->zwroc_taka_plansze(g->px-1,g->py,g->pz) && swiat->zwroc_taka_plansze(g->px-1,g->py,g->pz)->otoczenie[g->y][20]!=NULL&&swiat->zwroc_taka_plansze(g->px-1,g->py,g->pz)->otoczenie[g->y][20]->wysokosc>=g->wysokosc)return false;
                  }
         else if(strona=='g' || strona=='d'){
                  int start=g->y;if(strona=='d') ++start; else --start;
                  while((strona=='d'&&start<13)||(strona=='g'&&start>-1)){
                           if(p->otoczenie[start][g->x]!=NULL && p->otoczenie[start][g->x]->wysokosc>=g->wysokosc) return false;
                           if(strona=='d') ++start; else --start;
                  }
                  if(strona=='d' && swiat->zwroc_taka_plansze(g->px,g->py+1,g->pz) && swiat->zwroc_taka_plansze(g->px,g->py+1,g->pz)->otoczenie[0][g->x]!=NULL&&swiat->zwroc_taka_plansze(g->px,g->py+1,g->pz)->otoczenie[0][g->x]->wysokosc>=g->wysokosc)return false;
                  if(strona=='g' && swiat->zwroc_taka_plansze(g->px,g->py-1,g->pz) && swiat->zwroc_taka_plansze(g->px,g->py-1,g->pz)->otoczenie[12][g->x]!=NULL&&swiat->zwroc_taka_plansze(g->px,g->py-1,g->pz)->otoczenie[12][g->x]->wysokosc>=g->wysokosc)return false;
                  }
         return true;
}

int* Gracz::worn_items_graphics()
{
         al_lock_mutex(mutex_itemow_zalozonych);
         graph[0] = p_buty ? p_buty->get_co_to() : 0;
         graph[1] = p_plecak ? p_plecak->get_co_to() : 0;
         graph[2] = p_glowa ? p_glowa->get_co_to() : 0;
         graph[3] = p_spodnie ? p_spodnie->get_co_to() : 0;
         graph[4] = p_ramie ? p_ramie->get_co_to() : 0;
         graph[5] = p_rece ? p_rece->get_co_to() : 0;
         graph[6] = p_rekawice ? p_rekawice->get_co_to() : 0;
         graph[7] = p_korpus ? p_korpus->get_co_to() : 0;
         graph[8] = ilosc_kieszeni>'0' ? (p_kieszenl ? p_kieszenl->get_co_to() : 0) : 0;
         graph[9] = ilosc_kieszeni>'1' ? (p_kieszenp ? p_kieszenp->get_co_to() : 0) : 0;
         al_unlock_mutex(mutex_itemow_zalozonych);
         return graph;
}

void Gracz::zaktualizuj_przed_runda()
{
         al_lock_mutex(mutex_blokow); al_lock_mutex(mutex_unikow); al_lock_mutex(mutex_skupien);
         zablokowani.clear();
         uniknieci.clear();
         skupieni.clear();
         al_unlock_mutex(mutex_blokow); al_unlock_mutex(mutex_unikow); al_unlock_mutex(mutex_skupien);
}

std::vector<punkt> Gracz::blokowani()
{
         al_lock_mutex(mutex_blokow);
         std::vector<punkt> aaa(zablokowani.size());
         short int i=0;
         for(std::list<std::pair<Objekt*, short>>::iterator it = zablokowani.begin(); it!=zablokowani.end(); ++it)
         {
                  aaa[i++] = punkt((*it).first->x, (*it).first->y);
         }
         al_unlock_mutex(mutex_blokow);
         return aaa;
}

std::vector<punkt> Gracz::unikani()
{
         al_lock_mutex(mutex_unikow);
         std::vector<punkt> aaa(uniknieci.size());
         short int i=0;
         for(std::list<Objekt*>::iterator it = uniknieci.begin(); it!=uniknieci.end(); ++it)
         {
                  aaa[i++] = punkt((*it)->x, (*it)->y);
         }
         al_unlock_mutex(mutex_unikow);
         return aaa;
}

std::vector<punkt> Gracz::skupiani()
{
         al_lock_mutex(mutex_skupien);
         std::vector<punkt> aaa(skupieni.size());
         short int i=0;
         for(std::list<Objekt*>::iterator it = skupieni.begin(); it!=skupieni.end(); ++it)
         {
                  aaa[i++] = punkt((*it)->x, (*it)->y);
         }
         al_unlock_mutex(mutex_skupien);
         return aaa;
}

bool Gracz::jest_wsrod_blokow(Objekt *a)
{
         al_lock_mutex(mutex_blokow);
         for(std::list<std::pair<Objekt*, short>>::iterator it = zablokowani.begin(); it!=zablokowani.end(); ++it)
         {
                  if(a==(*it).first) {al_unlock_mutex(mutex_blokow); return true;}
         }
         al_unlock_mutex(mutex_blokow);
         return false;
}

bool Gracz::jest_wsrod_unikow(Objekt *a)
{
         al_lock_mutex(mutex_unikow);
         for(std::list<Objekt*>::iterator it = uniknieci.begin(); it!=uniknieci.end(); ++it)
         {
                  if(a==(*it)) {al_unlock_mutex(mutex_unikow); return true;}
         }
         al_unlock_mutex(mutex_unikow);
         return false;
}

bool Gracz::jest_wsrod_skupien(Objekt *a)
{
         al_lock_mutex(mutex_skupien);
         for(std::list<Objekt*>::iterator it = skupieni.begin(); it!=skupieni.end(); ++it)
         {
                  if(a==(*it)) {al_unlock_mutex(mutex_skupien); return true;}
         }
         al_unlock_mutex(mutex_skupien);
         return false;
}

void Gracz::dodaj_blok(Objekt *a, short blok)
{
         al_lock_mutex(mutex_blokow);
         zablokowani.push_back(std::pair<Objekt*, short>(a, blok));
         al_unlock_mutex(mutex_blokow);
}

void Gracz::dodaj_unik(Objekt *a)
{
         al_lock_mutex(mutex_unikow);
         uniknieci.push_back(a);
         al_unlock_mutex(mutex_unikow);
}

void Gracz::dodaj_skupienie(Objekt *a)
{
         al_lock_mutex(mutex_skupien);
         skupieni.push_back(a);
         al_unlock_mutex(mutex_skupien);
}

short Gracz::ile_bloku(Objekt *a)
{
         al_lock_mutex(mutex_blokow);
         for(std::list<std::pair<Objekt*, short>>::iterator it = zablokowani.begin(); it!=zablokowani.end(); ++it)
         {
                  if(a==(*it).first) {short h = (*it).second; al_unlock_mutex(mutex_blokow); return h;}
         }
         al_unlock_mutex(mutex_blokow);
         return 0;
}

int Gracz::zasieg_wzroku()
{
         int a=0;
         if(p_rece!=NULL && p_rece->czym_jest==8001) a+=8;
         else if(p_rece!=NULL && (p_rece->czym_jest==4255 ||  p_rece->czym_jest==4256)) a+=7;
         return 3+a;
}

int Gracz :: get_co_to()
{
         int a;
         if(otwiera) return 30;
         if(gwizdze) return 31;
         if(krzyczy) return 32;
         switch(postawa) {case 'r': a=10; break; case 's': a=16; break; case 'c': a=24; break;}
         switch(zwrot) {case 'd': a+=0;break; case 'g': a+=1;break; case 'l': a+=2;break; case 'p': a+=3;break;}
         return a;
}

void Gracz::uslysz_dzwiek(int dx,int dy,int rodzaj){}

void Gracz::uslysz_dzwiek(Plansza *a,int rodzaj){}

int najmniej_jeden(int a){if(a<1)return 1; else return a;}

int Gracz::uzycie_ubrania(Item **ubr, int obrazenia, char f)
{
         int a=0; if(f=='k') {a+=((Ubranie*)(p_glowa))->ochrona();if(p_glowa && ((p_glowa))->wykorzystaj()) { Item*a=((Ubranie*)(p_glowa));p_glowa=NULL;delete a;} }
         obrazenia=najmniej_jeden(obrazenia*(100-((Ubranie*)(*ubr))->ochrona()-a)/100);if(*ubr && ((Ubranie*)(*ubr))->wykorzystaj()){Item*a=((Ubranie*)(*ubr));*ubr=NULL;delete a;}return obrazenia;
}

short Gracz::stan_nog()
{
         bool A=l_udo.ile_hp()<5,B=p_udo.ile_hp()<5,C=l_golen.ile_hp()<5,D=p_golen.ile_hp()<5;
         if((A&&C&&(B||D))||(B&&D&&(A||C))) return 1; ///tylko crawl i za 7
         else if((A&&C)||(B&&D)) return 2; ///tylko crawl
         else if((A&&B)||(A&&D)||(C&&B)||(C&&D)) return 3;///bieganie za 5 sneak za 6 crawl za 7
         else if(A||B||C||D) return 4;///bieganie za 4
         return 0;
}

short Gracz::wart_zajecia()
{
         return 4;
}

int Gracz::obrazenia(Bron *b)
{
         float wsp = postawa == 'r' ? 1 : (postawa == 's' ? 0.8 : 0.6);
         return wsp * b->get_obrazenia()*(70+(b->dwureczna() ? suma_hp_rak() : 15+(ramie_l.ile_hp()+l_dlon.ile_hp()>p_dlon.ile_hp()+ramie_p.ile_hp() ? ramie_l.ile_hp()+l_dlon.ile_hp() : p_dlon.ile_hp()+ramie_p.ile_hp())))/100;
}

int Gracz::szansa(Bron *b)
{
         return b->get_szansa()*(70+(b->dwureczna() ? suma_hp_rak() : 15+(ramie_l.ile_hp()+l_dlon.ile_hp()>p_dlon.ile_hp()+ramie_p.ile_hp() ? ramie_l.ile_hp()+l_dlon.ile_hp() : p_dlon.ile_hp()+ramie_p.ile_hp())))/100;
}

what_happened Gracz::zostan_uderzony(int obrazenia, int kto, int ekstra, short wsp_zajecia)
{
         if(*Objekt::args->godmode) return success;
         if(this==NULL) {std::cout<<"NULL na Gracz::zostan_uderzony"; return dead;}
         p_ruchu -= wsp_zajecia; if(p_ruchu<3) p_ruchu=3;
         int aaa=losuj(1,10); bool br=false;
         if(kto==-3) aaa = losuj(9, 10);
         else if(kto==-4) aaa = losuj(7, 10);

         while(1)
         {
                  switch(aaa)
                  {
                           case 1:if(klatka.ile_hp()>0){obrazenia=uzycie_ubrania(&p_korpus, obrazenia,'k');klatka.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 2:if(brzuch.ile_hp()>0){obrazenia=uzycie_ubrania(&p_korpus, obrazenia);brzuch.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 3:if(ramie_l.ile_hp()>0){obrazenia=uzycie_ubrania(&p_korpus, obrazenia);ramie_l.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 4:if(ramie_p.ile_hp()>0){obrazenia=uzycie_ubrania(&p_korpus, obrazenia);ramie_p.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 5:if(l_dlon.ile_hp()>0){obrazenia=uzycie_ubrania(&p_rekawice, obrazenia); l_dlon.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 6:if(p_dlon.ile_hp()>0){obrazenia=uzycie_ubrania(&p_rekawice, obrazenia);p_dlon.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 7:if(l_udo.ile_hp()>0){obrazenia=uzycie_ubrania(&p_spodnie, obrazenia); l_udo.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 8:if(p_udo.ile_hp()>0){obrazenia=uzycie_ubrania(&p_spodnie, obrazenia); p_udo.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 9:if(l_golen.ile_hp()>0){obrazenia=uzycie_ubrania(&p_buty, obrazenia); l_golen.zadaj_obrazenia(obrazenia); br=true;break;}
                           case 10:if(p_golen.ile_hp()>0){obrazenia=uzycie_ubrania(&p_buty, obrazenia);p_golen.zadaj_obrazenia(obrazenia); br=true;break;}
                  }
                  if(br)break; aaa=1;br=true;
         }
         animacja_damage(obrazenia);
         if(czy_zyje())
         {
                  if(stan_nog()==1 || stan_nog()==2) postawa='c';
         }
         else return dead;
         return success;
}

bool Gracz::rusz_sie()
{
         throw "gracz jest obiektem !?";
}

void Gracz::patrz(){}

void Gracz :: zapisz(std::ofstream *ofs)
{

}

void Gracz::dzwiek_przesuniecia(int xx, int yy)
{
         if(postawa=='r')wydaj_dzwiek(false, 2, xx,yy,10);
}

void Gracz::strzel(Objekt*a)
{
         if(mozna_strzelic(a))
         {
                  animacja_strzalu(x+args->X_kratka/2, y+args->Y_kratka/2, a->x+args->X_kratka/2, a->y+args->Y_kratka/2, 1);
         }
}

bool Gracz::odejmij_hp_z_powodu_niedozywienia()
{
         if(klatka.ile_hp()<brzuch.ile_hp()) klatka.zadaj_obrazenia(1); else brzuch.zadaj_obrazenia(1);
         return czy_zyje();
}

bool Gracz::czy_zyje()
{
         if(klatka.ile_hp()<=0 || brzuch.ile_hp()<=0)
         {
                  args->otwarte_menu_anatomii=true;Sleep(3000);args->otwarte_menu_anatomii=true;

                  Plansza*p=swiat->zwroc_taka_plansze_TYLKO(px,py,pz);
                  przestan_byc_ofiara(1);
                  if(pozostalo_snu<=0)p->otoczenie[y][x]=NULL; ///czy on wgl jest na planszy
                  Srodowisko::dodaj_objekt((new Zwloki(czym_jest,0,x,y,px,py,pz)),x,y,p);
                  p->zaktualizuj_widoki(x,y,x,y);

                  if(p->ziemia[y][x]==NULL) p->ziemia[y][x]=new Kontener<Item>();
                  Kontener<Item>*poz=new Kontener<Item>;
                  poz->dodaj_obiekt(p_rece);poz->dodaj_obiekt(p_korpus);poz->dodaj_obiekt(p_spodnie);poz->dodaj_obiekt(p_buty);poz->dodaj_obiekt(p_rekawice);
                  poz->dodaj_obiekt(p_kieszenp);poz->dodaj_obiekt(p_kieszenl);poz->dodaj_obiekt(p_ramie);poz->dodaj_obiekt(p_glowa);poz->dodaj_obiekt(p_plecak);
                  p->ziemia[y][x]->dodaj_obiekty(poz); poz->wywal_wszystkie_objekty(); delete poz;
                  p_glowa=NULL;p_korpus=NULL;p_spodnie=NULL;p_buty=NULL;p_ramie=NULL;p_plecak=NULL;p_rece=NULL;p_rekawice=NULL;p_kieszenl=NULL;p_kieszenp=NULL;
                  if(p->ziemia[y][x]->ilosc==0) {Kontener<Item>* a=p->ziemia[y][x];p->ziemia[y][x]=NULL; delete a;}

                  if(swiat->aktualny==this) swiat->aktualny=NULL;
                  swiat->gracz->usun_objekt(this);

                  int r=swiat->promien_obszaru_zywego;

                  for(int i=px-r;i<=px+r;++i)
                  {
                           for(int j=py-r-1;j<=py+r+1;++j)
                           {
                                    if(i>-1 && j>-1 && i<swiat->w_x && j<swiat->w_y)
                                    {
                                             if(swiat->zwroc_taka_plansze(i,j,pz) && !swiat->w_zasiegu_gracza(i,j,pz))
                                             {
                                                      swiat->stworz_strefe(i,j,pz);
                                             }
                                    }
                           }
                  }
                  smierc();
                  return false;
         }
         return true;
}

bool Gracz::wstan_ze_snu()
{
         pozostalo_snu = 0;

         ///najpierw tam gdzie stal
         ///potem prostopadle
         ///potem ukosy
         ///potem lez dalej
         px = gdzie_spi->px; pz = gdzie_spi->pz; py = gdzie_spi->py;
         Plansza *pl = swiat->zwroc_taka_plansze_TYLKO(px,py,pz);
         if(pl->otoczenie[y][x]==NULL && zaraz_obok(x,y,gdzie_spi->x,gdzie_spi->y))
         {
                  pl->otoczenie[y][x] = this;
         }
         else if(Strefa::w_planszy(gdzie_spi->x+1, gdzie_spi->y) && pl->otoczenie[gdzie_spi->y][gdzie_spi->x+1]==NULL)
         {x = gdzie_spi->x+1; pl->otoczenie[y][x] = this;}
         else if(Strefa::w_planszy(gdzie_spi->x-1, gdzie_spi->y) && pl->otoczenie[gdzie_spi->y][gdzie_spi->x-1]==NULL)
         {x = gdzie_spi->x-1; pl->otoczenie[y][x] = this;}
         else if(Strefa::w_planszy(gdzie_spi->x, gdzie_spi->y+1) && pl->otoczenie[gdzie_spi->y+1][gdzie_spi->x]==NULL)
         {x = gdzie_spi->y+1; pl->otoczenie[y][x] = this;}
         else if(Strefa::w_planszy(gdzie_spi->x, gdzie_spi->y-1) && pl->otoczenie[gdzie_spi->y-1][gdzie_spi->x]==NULL)
         {x = gdzie_spi->y-1; pl->otoczenie[y][x] = this;}

         else if(Strefa::w_planszy(gdzie_spi->x-1, gdzie_spi->y-1) && pl->otoczenie[gdzie_spi->y-1][gdzie_spi->x-1]==NULL)
         {x = gdzie_spi->x-1; y = gdzie_spi->y-1; pl->otoczenie[y][x] = this;}
         else if(Strefa::w_planszy(gdzie_spi->x+1, gdzie_spi->y+1) && pl->otoczenie[gdzie_spi->y+1][gdzie_spi->x+1]==NULL)
         {x = gdzie_spi->x+1; y = gdzie_spi->y+1; pl->otoczenie[y][x] = this;}
         else if(Strefa::w_planszy(gdzie_spi->x-1, gdzie_spi->y+1) && pl->otoczenie[gdzie_spi->y+1][gdzie_spi->x-1]==NULL)
         {x = gdzie_spi->x-1; y = gdzie_spi->y+1; pl->otoczenie[y][x] = this;}
         else if(Strefa::w_planszy(gdzie_spi->x+1, gdzie_spi->y-1) && pl->otoczenie[gdzie_spi->y-1][gdzie_spi->x+1]==NULL)
         {x = gdzie_spi->x+1; y = gdzie_spi->y-1; pl->otoczenie[y][x] = this;}

         else
         {
                  pozostalo_snu = 1; return false;
         }
         ukryty = false;
         if(gdzie_spi->czym_jest) ((Szalas*)gdzie_spi)->kto_spi = NULL;
         pl->ma_gracza = true;
         wyjdz_animacja(gdzie_spi->x, gdzie_spi->y);
         gdzie_spi = NULL;
         return true;
}

Gracz::~Gracz()
{
         al_destroy_mutex(mutex_blokow);
         al_destroy_mutex(mutex_unikow);
         al_destroy_mutex(mutex_skupien);
}

int Gracz::kamuflaz()
{
         return ((Ubranie*)p_glowa)->kamuflaz()+((Ubranie*)p_korpus)->kamuflaz()+((Ubranie*)p_spodnie)->kamuflaz()
         +((Ubranie*)p_buty)->kamuflaz()+((Ubranie*)p_rekawice)->kamuflaz()+((Plecak*)p_plecak)->kamuflaz();
}

int Gracz::ocieplenie()
{
         return ((Ubranie*)p_glowa)->cieplo()+((Ubranie*)p_korpus)->cieplo()+((Ubranie*)p_spodnie)->cieplo()
         +((Ubranie*)p_buty)->cieplo()+((Ubranie*)p_rekawice)->cieplo();
}

void Gracz::dostan_item(Item*a)
{
         if(a==NULL) return;
         if(p_rece==NULL) {p_rece=a; return;}
         Przedmiot *b=new Przedmiot(a);
         if(a->mozna_do_plecaka() && p_plecak!=NULL && ((Plecak*)p_plecak)->s->wloz_przedmiot(b)) return; else {delete b;}
         swiat->zwroc_taka_plansze_TYLKO(px,py,pz)->rzuc_na_ziemie(x,y,a);
}

short Gracz::latwopalnosc() {return 0;}

bool Gracz::posiada_przedmiot(int jaki, short w_ilosci, bool patrz_na_ziemie)
{
         short ilosc = 0;
         if(p_rece!=NULL && p_rece->czym_jest==jaki) ilosc++;
         if(jaki>5000 && jaki<7500)
         {
                  if(p_glowa!=NULL && p_glowa->czym_jest==jaki) ilosc++;
                  if(p_korpus!=NULL && p_korpus->czym_jest==jaki) ilosc++;
                  if(p_spodnie!=NULL && p_spodnie->czym_jest==jaki) ilosc++;
                  if(p_buty!=NULL && p_buty->czym_jest==jaki) ilosc++;
                  if(p_rekawice!=NULL && p_rekawice->czym_jest==jaki) ilosc++;
         }
         if(p_ramie!=NULL && p_ramie->czym_jest==jaki) ilosc++;
         if(p_kieszenl!=NULL && p_kieszenl->czym_jest==jaki) ilosc++;
         if(p_kieszenp!=NULL && p_kieszenp->czym_jest==jaki) ilosc++;

         if(p_plecak!=NULL)
         {
                  //if(p_plecak->czym_jest==jaki) ilosc++;
                  //else
                  {
                           Skrzynka *s = ((Plecak*)p_plecak)->s;
                           for(short i=0; i<s->lista_itemow()->ilosc; ++i)
                           {
                                    if(s->lista_itemow()->obiekt[i]->czym_jest == jaki)
                                             ilosc++;
                           }
                  }
         }
         if(patrz_na_ziemie)
         {
                  if(otwarta!=NULL)
                  {
                           for(short i=0; i<otwarta->lista_itemow()->ilosc; ++i)
                                    if(otwarta->lista_itemow()->obiekt[i]->czym_jest == jaki)
                                             ilosc++;
                  }
         }
         return ilosc >= w_ilosci;
}

Item** Gracz::odbierz_przedmiot(int jaki, short w_ilosci, bool z_ziemii_tez)
{
         if(!posiada_przedmiot(jaki, w_ilosci, z_ziemii_tez)) return NULL;

         Item **tab = new Item*[w_ilosci+1];
         tab[w_ilosci] = NULL;
         short licznik = 0;

         if(p_rece!=NULL && p_rece->czym_jest==jaki) {tab[licznik++] = p_rece; p_rece=NULL; if(licznik==w_ilosci) return tab;}
         if(jaki>5000 && jaki<7500)
         {
                  if(p_glowa!=NULL && p_glowa->czym_jest==jaki) {tab[licznik++] = p_rece; p_rece=NULL; if(licznik==w_ilosci) return tab;}
                  if(p_korpus!=NULL && p_korpus->czym_jest==jaki) {tab[licznik++] = p_korpus; p_korpus=NULL; if(licznik==w_ilosci) return tab;}
                  if(p_spodnie!=NULL && p_spodnie->czym_jest==jaki) {tab[licznik++] = p_spodnie; p_spodnie=NULL; if(licznik==w_ilosci) return tab;}
                  if(p_buty!=NULL && p_buty->czym_jest==jaki) {tab[licznik++] = p_buty; p_buty=NULL; if(licznik==w_ilosci) return tab;}
                  if(p_rekawice!=NULL && p_rekawice->czym_jest==jaki) {tab[licznik++] = p_rekawice; p_rekawice=NULL; if(licznik==w_ilosci) return tab;}
         }
         if(p_ramie!=NULL && p_ramie->czym_jest==jaki) {tab[licznik++] = p_ramie; p_ramie=NULL; if(licznik==w_ilosci) return tab;}
         if(p_kieszenl!=NULL && p_kieszenl->czym_jest==jaki) {tab[licznik++] = p_kieszenl; p_kieszenl=NULL; if(licznik==w_ilosci) return tab;}
         if(p_kieszenp!=NULL && p_kieszenp->czym_jest==jaki) {tab[licznik++] = p_kieszenp; p_kieszenp=NULL; if(licznik==w_ilosci) return tab;}

         if(p_plecak!=NULL)
         {
                  if(p_plecak->czym_jest==jaki) {tab[licznik++] = p_plecak; p_plecak=NULL; if(licznik==w_ilosci) return tab;}
                  else
                  {
                           Skrzynka *s = ((Plecak*)p_plecak)->s;
                           for(short i=0; i<s->lista_itemow()->ilosc; ++i)
                           {
                                    if(s->lista_itemow()->obiekt[i]->czym_jest == jaki)
                                    {
                                             tab[licznik++] = s->lista_itemow()->obiekt[i];
                                             Przedmiot *aaa = s->zwroc_przedmiot(s->lista_itemow()->obiekt[i]);
                                             s->wyjmij_przedmiot(aaa); delete aaa;
                                             if(licznik==w_ilosci) return tab;
                                    }
                           }
                  }
         }
         if(z_ziemii_tez)
         {
                  if(otwarta!=NULL)
                  {
                           for(short i=0; i<otwarta->lista_itemow()->ilosc; ++i)
                                    if(otwarta->lista_itemow()->obiekt[i]->czym_jest == jaki)
                                    {
                                             tab[licznik++] = otwarta->lista_itemow()->obiekt[i];
                                             Przedmiot *aaa = otwarta->zwroc_przedmiot(otwarta->lista_itemow()->obiekt[i]);
                                             otwarta->wyjmij_przedmiot(aaa); delete aaa;
                                             if(licznik==w_ilosci) return tab;
                                    }
                  }
         }
         return tab;
}

bool Gracz::ma_umiejetnosc(short jaka)
{
         return false;
}

bool Gracz::jest_obok_objektu(short jakiego)
{
         short t = jakiego;
         if(jakiego<0)
         {
                  switch(jakiego)
                  {
                           default: jakiego*=-1;
                  }
         }
         Plansza *p = swiat->zwroc_taka_plansze_TYLKO(px,py,pz);
         for(short i=x-1; i<=x+1; ++i)
         {
                  for(short j=y-1; j<=y+1; ++j)
                  {
                           if(Strefa::w_planszy(i, j))
                           {
                                    if(p->otoczenie[j][i]!=NULL && p->otoczenie[j][i]->czym_jest==jakiego)
                                    {
                                             switch(t)
                                             {
                                                      default: return true;
                                             }
                                    }
                                    else if(p->srodowisko[j][i]!=NULL && p->srodowisko[j][i]->wierzch()!=NULL && p->srodowisko[j][i]->wierzch()->czym_jest==jakiego)
                                    {
                                             switch(t)
                                             {
                                                      case -24:
                                                      {
                                                               if(((Ognisko*)p->srodowisko[j][i]->wierzch())->getPaliSie())
                                                                        return true;
                                                               else break;
                                                      }
                                                      default: return true;
                                             }
                                    }
                           }
                  }
         }
         return false;
}

void Gracz::wykorzystaj_item(short jaki)
{

}
