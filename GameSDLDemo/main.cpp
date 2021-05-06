#include "Common_Function.h"
#include "MainObject.h"
#include "ThreatObject.h"
#include "TextObject.h"

#undef main


TTF_Font* g_font_text = NULL;

// GAME MAY BAY CHIEN DAU
// ham Init khoi tao cac thuoc tinh ban dau 
bool Init(){
  if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
    return false;
  g_screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE);
  if (g_screen == NULL)
    return false;
  if (TTF_Init() == -1)
	return false;
  g_font_text = TTF_OpenFont("Another_Typewriter.ttf", 20);
  if (g_font_text == NULL)
	return false;

  return true;
}


int main(int arc, char* argv[]){
	int bkgn_x = 0;
	bool is_quit = false;
	if (Init() == false)
	   return 0;
 
	g_bkground = SDLCommonFunc::LoadImage("backg4800.png");
	if (g_bkground == NULL)	
		return 0;
	
	//thoi gian
	TextObject time_game;
	time_game.SetColor(TextObject::BLACK_TEXT);
	
	//Diem
	TextObject mark_game;
	mark_game.SetColor(TextObject::RED_TEXT);

	//nhan vat chinh
	MainObject plane_object;
	plane_object.SetRect(100, 200);
	bool ret = plane_object.LoadImg("plane_fly.png");
	if (!ret){return 0;}
	
	// cac moi hiem hoa
	ThreatObject *p_threats = new ThreatObject[NUM_THREATS];
	for (int i= 0; i < NUM_THREATS; i++)
	{
		ThreatObject* p_threat = (p_threats + i);
		if(p_threat){
			ret = p_threat->LoadImg("af1.png");
			if (ret == false) return 0;

			int rand_y = rand()%400;
			if (rand_y > SCREEN_HEIGHT - 200)
			{
				rand_y = SCREEN_HEIGHT * 0.3;
			}

			p_threat->SetRect(SCREEN_WIDTH + i*400, rand_y);
			p_threat->set_x_val(3);
			AmoObject* p_amo = new AmoObject();
			p_threat->InitAmo(p_amo);
		}
		
	}
	
	unsigned int mark_value = 0;

	int ret_menu = SDLCommonFunc::ShowMenu(g_screen, g_font_text);
	if (ret_menu == 1){
		is_quit = true;
	}
	while (!is_quit) 
	{
		while (SDL_PollEvent(&g_even)) 
		{
		  if (g_even.type == SDL_QUIT)
		  {
			is_quit = true;
			break;
		  }
		  plane_object.HandleInputAction(g_even);
		}
		// Apply Background
		bkgn_x -= 1;
		SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x, 0);
		SDLCommonFunc::ApplySurface(g_bkground, g_screen, bkgn_x + SCREEN_WIDTH, 0);
		if (bkgn_x <= -SCREEN_WIDTH){
			bkgn_x = 0;
		}
		//Implement main object
		plane_object.HandleMove();
		plane_object.Show(g_screen);
		plane_object.MakeAmo(g_screen);

		//Implement Threats object
		for (int i=0; i<NUM_THREATS; i++)
		{
			ThreatObject* p_threat = (p_threats + i);
			if (p_threat){
				p_threat->HandleMove(SCREEN_WIDTH, SCREEN_HEIGHT);
				p_threat->Show(g_screen);
				p_threat->MakeAmo(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);	

				//Kiem tra va cham giua main va dan dich
				bool is_coll = false;
				std::vector<AmoObject*> amo_arr = p_threat->GetAmoList();
				for (int am = 0; am < amo_arr.size(); am++)
				{
					AmoObject* p_amo = amo_arr.at(am);
					if (p_amo)
					{
						is_coll = SDLCommonFunc::CheckCollision(p_amo->GetRect(), plane_object.GetRect());
						if(is_coll == true)
						{
							p_threat-> RetsetAmo(p_amo);
							break;
						}
					}
				}
				
				//kiem tra va cham giua main voi may bay dich
				bool is_col = SDLCommonFunc::CheckCollision(plane_object.GetRect(), p_threat->GetRect());
				
				if (is_col || is_coll)
				{
					if(MessageBox(NULL, L"GAME OVER!", L"ResultGame", MB_OK) ==IDOK)
					{
						delete [] p_threats;
						SDLCommonFunc::CleanUp();
						SDL_Quit();
						return 1;
					}
				}
				//kiem tra va cham giua dan main voi may bay dich
				std::vector<AmoObject*> amo_list = plane_object.GetAmoList();
				for (int im = 0; im < amo_list.size(); im++)
				{
					AmoObject* p_amo = amo_list.at(im);
					if (p_amo != NULL)
					{
						bool ret_col = SDLCommonFunc::CheckCollision(p_amo->GetRect(), p_threat-> GetRect());
						if (ret_col)
						{
							mark_value++;
							p_threat->Reset(SCREEN_WIDTH + i*400);
							plane_object.RemoveAmo(im);
						}
					}
				}

			}
		}
		//Hien thi thoi gian ra man hinh
		std::string str_time = "Time : ";
		Uint32 time_val = SDL_GetTicks()/1000;
		std::string str_val = std::to_string(time_val);
		str_time += str_val;
		time_game.SetText(str_time);
		time_game.SetRect(SCREEN_WIDTH - 200, 5);
		time_game.CreateGameText(g_font_text, g_screen);

		//in diem ra man hinh
		std::string val_str_mark = std::to_string(mark_value);
		std::string strMark("Mark: ");
		strMark += val_str_mark;
		mark_game.SetText(strMark);
		mark_game.CreateGameText(g_font_text, g_screen);
		//Update screen
		if ( SDL_Flip(g_screen) == -1)
		return 0;
	 }

	 delete [] p_threats;
	 SDLCommonFunc::CleanUp();
	 SDL_Quit();
	 return 1;
	
}
