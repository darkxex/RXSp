#ifdef __SWITCH__
#include <unistd.h>
#include <switch.h>
#include <dirent.h>
#endif 
#ifndef __SWITCH__
#define _CRT_SECURE_NO_WARNINGS
#endif


//test

#include <SDL.h>
#include <SDL_thread.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>
#include <curl/curl.h>

#include <math.h> 

#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <math.h>  
#include <Vector>
#include <dirent.h>
#include <algorithm>
// Empty strings are invalid.
#ifdef __SWITCH__
SwkbdTextCheckResult Keyboard_ValidateText(char *string, size_t size) {
	if (strcmp(string, "") == 0) {
		strncpy(string, "The name cannot be empty.", size);
		return SwkbdTextCheckResult_Bad;
	}

	return SwkbdTextCheckResult_OK;
}

const char *Keyboard_GetText(const char *guide_text, const char *initial_text) {
	Result ret = 0;
	SwkbdConfig swkbd;
	static char input_string[256];

	if (R_FAILED(ret = swkbdCreate(&swkbd, 0))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdConfigMakePresetDefault(&swkbd);

	if (strlen(guide_text) != 0)
		swkbdConfigSetGuideText(&swkbd, guide_text);

	if (strlen(initial_text) != 0)
		swkbdConfigSetInitialText(&swkbd, initial_text);

	swkbdConfigSetTextCheckCallback(&swkbd, Keyboard_ValidateText);

	if (R_FAILED(ret = swkbdShow(&swkbd, input_string, sizeof(input_string)))) {
		swkbdClose(&swkbd);
		return "";
	}

	swkbdClose(&swkbd);
	return input_string;
}
#endif 




bool	isFileExist(const char *file)
{
	struct stat	st = { 0 };

	if (stat(file, &st) == -1) {
		return (false);
	}

	return (true);
}

//////////////////////////////////aquí empieza el pc.
//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
enum states { selectmanga, readmanga,selectconsole };
enum statesrom { snesrom, gbarom,gbcrom,gbrom,nesrom,genesisrom,psxntsc,psxntscj,psxpal,n64rom,pcenginerom,neogeorom,mastersystemrom,dreamcastrom,commodore64rom };
int stateromnow = snesrom;
int statenow = selectconsole;
std::string  urltodownload = "";
int porcendown = 0;
int sizeestimated = 0;
std::string temporallink = "";
bool ahorro = false;
int cancelcurl = 0;
bool preview = false;
int searchchapter = 0;
int selectchapter = 0;
int selectpage = 0;
float neutralsize = 1;
int widthnew = 0;
int heightnew = 0;
int cascademode = 0;
bool helppage = false;
bool existfoldermain = true;
int basexmain = 20;
int baseymain = 60;
bool cascadeactivated = false;
bool seevertical = true;
bool fulldownloaded = false;
bool loading = true;
std::string containrom = "mario";
std::string baserom = "https://the-eye.eu/public/rom/SNES/";
std::string typerom = "Super Nintendo";
std::string urldownload = "";
std::string tosearch = "";
#ifdef __SWITCH__
std::string directoryrom = "sdmc:/RomsXShop/SNES/";
#else
std::string directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/SNES/";
#endif // SWITCH


//Texture wrapper class
class LTexture
{
public:
	//Initializes variables
	LTexture();

	//Deallocates memory
	~LTexture();

	//Loads image at specified path
	bool loadFromFile(std::string path);
	bool loadFromFileCustom(std::string path,int h, int w);
	//Creates image from font string
	bool loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor);
	//Creates image from font string
	bool loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size);
	//Deallocates texture
	void free();

	//Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	//Set blending
	void setBlendMode(SDL_BlendMode blending);

	//Set alpha modulation
	void setAlpha(Uint8 alpha);

	//Renders texture at given point
	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	//Renders texture at given point
	void customrender(int x, int y,int w,int h, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	//Gets image dimensions
	int getWidth();
	int getHeight();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;

	//Image dimensions
	int mWidth;
	int mHeight;
};

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Globally used font
TTF_Font *gFont = NULL;
TTF_Font *gFontcapit = NULL;
TTF_Font *gFont2 = NULL;
TTF_Font *gFont3 = NULL;
//Rendered texture
LTexture gTextTexture;
LTexture gTextTexture2;
LTexture Farest;
LTexture Heart;
LTexture Pagemanga;
LTexture lupa;
LTexture TPreview;

std::vector<LTexture> arraypage;
LTexture::LTexture()
{
	//Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	//Deallocate
	free();
}

bool LTexture::loadFromFile(std::string path)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load((path.c_str()));
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}


bool LTexture::loadFromFileCustom(std::string path, int h, int w)
{
	//Get rid of preexisting texture
	free();

	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = w;
			mHeight = h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	mTexture = newTexture;
	return mTexture != NULL;
}
bool LTexture::loadFromRenderedText(TTF_Font *fuente, std::string textureText, SDL_Color textColor)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Blended(fuente, textureText.c_str(), textColor);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}
bool LTexture::loadFromRenderedTextWrap(TTF_Font *fuente, std::string textureText, SDL_Color textColor, Uint32 size)
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended_Wrapped(fuente, textureText.c_str(), textColor,size);

	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	//Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	//Free texture if it exists
	if (mTexture != NULL)
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	//Modulate texture rgb
	SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	//Set blending function
	SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	//Modulate texture alpha
	SDL_SetTextureAlphaMod(mTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}
void LTexture::customrender(int x, int y, int w,int h, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	//Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, w,h };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}





void close()
{
	//Free loaded images
	gTextTexture.free();
	gTextTexture2.free();
	Farest.free();
	Heart.free();
	TPreview.free();
	Pagemanga.free();
	lupa.free();
	for (int x = 0; x < arraypage.size(); x++)
	{
		arraypage[x].free();
	}
	//Free global font
	TTF_CloseFont(gFont);
	gFont = NULL;

	TTF_CloseFont(gFontcapit);
	gFontcapit = NULL;
	TTF_CloseFont(gFont2);
	gFont2 = NULL;
	TTF_CloseFont(gFont3);
	gFont3 = NULL;
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}


void replace(std::string& subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}


void mayus(std::string &s)
{
	bool cap = true;

	for (unsigned int i = 0; i <= s.length(); i++)
	{
		if (isalpha(s[i]) && cap == true)
		{
			s[i] = toupper(s[i]);
			cap = false;
		}
		else if (isspace(s[i]))
		{
			cap = true;
		}
	}
}
int progress_func(void* ptr, double TotalToDownload, double NowDownloaded,
	double TotalToUpload, double NowUploaded)
{


	// ensure that the file to be downloaded is not empty
	// because that would cause a division by zero error later on
	if (TotalToDownload <= 0.0) {
		return 0;
	}

	// how wide you want the progress meter to be
	int totaldotz = 20;
	double fractiondownloaded = NowDownloaded / TotalToDownload;
	// part of the progressmeter that's already "full"
	int dotz = round(fractiondownloaded * totaldotz);

	// create the "meter"
	int ii = 0;
	porcendown = fractiondownloaded * 100;
	sizeestimated = TotalToDownload;
	printf("%3.0f%% [", fractiondownloaded * 100);
	// part  that's full already
	for (; ii < dotz; ii++) {
		printf("=");
	}
	// remaining part (spaces)
	for (; ii < 20; ii++) {
		printf(" ");
	}
	// and back to line begin - do not forget the fflush to avoid output buffering problems!
	printf("]\r");
	fflush(stdout);
	// if you don't return 0, the transfer will be aborted - see the documentation
	if (cancelcurl == 1)
	{
		return 1;
	}

	return 0;

}
std::string urlDecode(std::string &SRC) {
	std::string ret;
	char ch;
	int i, ii;
	for (i = 0; i < SRC.length(); i++) {
		if (int(SRC[i]) == 37) {
			sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
			ch = static_cast<char>(ii);
			ret += ch;
			i = i + 2;
		}
		else {
			ret += SRC[i];
		}
	}
	return (ret);
}
std::string gethtml(std::string enlace)
{

	CURL *curl;
	CURLcode res;
	std::string Buffer;

	curl = curl_easy_init();
	if (curl) {

		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &Buffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		return Buffer;
	}
}
void downloadfile(std::string enlace, std::string directorydown)
{
	
	CURL *curl;
	FILE *fp;
	CURLcode res;

	curl = curl_easy_init();
	if (curl) {

		fp = fopen(directorydown.c_str(), "wb");
		curl_easy_setopt(curl, CURLOPT_URL, enlace.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		// Install the callback function
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_func);
		res = curl_easy_perform(curl);
		/* always cleanup */
		curl_easy_cleanup(curl);
		fclose(fp);
	}
}







std::vector<std::string> arraymain;
std::vector<std::string> arraychapter;
std::vector<std::string> arraynamedown;
std::vector<std::string> arrayconsole;
int downloaddata(void* data)
{
	//	
	std::cout << arraynamedown[selectchapter] << std::endl;
	std::cout << arraychapter[selectchapter] << std::endl;
	std::string namedownload = urlDecode(arraynamedown[selectchapter]);


	std::string directorydownload = directoryrom + namedownload;


	std::cout << namedownload << std::endl;
	downloadfile(arraychapter[selectchapter], directorydownload);
	return 0;
}

void reloadrom()
{
	arraymain.clear();
	arraychapter.clear();
	arraynamedown.clear();
	std::string sourceRoms = gethtml(baserom);
	int initcode = sourceRoms.find("<h1>Index of /public/rom/");
	int endcode = sourceRoms.find("</body>", initcode);
	int initroms = 0;
	int endroms;
	int initderoms, endderoms;
	sourceRoms = sourceRoms.substr(initcode, endcode - initcode);
	while (initroms != -1) {
		initroms = sourceRoms.find("<a href=\"", initroms);
		if (initroms == -1) { break; }
		initroms = initroms + 9;
		endroms = sourceRoms.find("\"", initroms);
		initderoms = sourceRoms.find("\">", endroms);
		initderoms = initderoms + 2;
		endderoms = sourceRoms.find("</a>", initderoms);
		//std::cout << baserom << sourceRoms.substr(initroms,endroms - initroms) << std::endl;
		if (sourceRoms.substr(initroms, endroms - initroms) != ("../"))
		{
			std::string completenamerom = sourceRoms.substr(initderoms, endderoms - initderoms);
			replace(completenamerom, "&gt;", "");
			replace(completenamerom, "&amp;", "");
			std::string completeurlrom = baserom + sourceRoms.substr(initroms, endroms - initroms);
			arraymain.push_back(completenamerom);
			arraychapter.push_back(completeurlrom);
			arraynamedown.push_back(sourceRoms.substr(initroms, endroms - initroms));
		}
		initroms++;
}}
int reloadromdata(void* data)
{
	
	std::string sourceRoms = gethtml(baserom);
	int initcode = sourceRoms.find("<h1>Index of /public/rom/");
	int endcode = sourceRoms.find("</body>", initcode);
	int initroms = 0;
	int endroms;
	int initderoms, endderoms;
	sourceRoms = sourceRoms.substr(initcode, endcode - initcode);
	loading = false;
	while (initroms != -1) {
		initroms = sourceRoms.find("<a href=\"", initroms);
		if (initroms == -1) { break; }
		initroms = initroms + 9;
		endroms = sourceRoms.find("\"", initroms);
		initderoms = sourceRoms.find("\">", endroms);
		initderoms = initderoms + 2;
		endderoms = sourceRoms.find("</a>", initderoms);
		//std::cout << baserom << sourceRoms.substr(initroms,endroms - initroms) << std::endl;
		if (sourceRoms.substr(initroms, endroms - initroms) != ("../") && sourceRoms.substr(initroms, endroms - initroms) != ("Nintendo%20Gameboy%20Advance%20Video/") && sourceRoms.substr(initroms, endroms - initroms) != ("Extras/") && sourceRoms.substr(initroms, endroms - initroms) != ("links.txt"))
		{
			std::string completenamerom = sourceRoms.substr(initderoms, endderoms - initderoms);
			replace(completenamerom, "&gt;", "");
			replace(completenamerom, "&amp;", "");
			std::string completeurlrom = baserom + sourceRoms.substr(initroms, endroms - initroms);
			arraymain.push_back(completenamerom);
			arraychapter.push_back(completeurlrom);
			arraynamedown.push_back(sourceRoms.substr(initroms, endroms - initroms));
		}
		initroms++;
	}
	
	
	return 0;
}
int case_insensitive_match(std::string s1, std::string s2) {
	//convert s1 and s2 into lower case strings
	transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
	transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
	if (s1.find(s2) != -1)
		return 1; //The strings are same
	return 0; //not matched
}
void reloadromfiltred(std::string filtred)
{
	arraymain.clear();
	arraychapter.clear();
	arraynamedown.clear();
	std::string sourceRoms = gethtml(baserom);
	int initcode = sourceRoms.find("<h1>Index of /public/rom/");
	int endcode = sourceRoms.find("</body>", initcode);
	int initroms = 0;
	int endroms;
	int initderoms, endderoms;
	sourceRoms = sourceRoms.substr(initcode, endcode - initcode);
	while (initroms != -1) {
		initroms = sourceRoms.find("<a href=\"", initroms);
		if (initroms == -1) { break; }
		initroms = initroms + 9;
		endroms = sourceRoms.find("\"", initroms);
		initderoms = sourceRoms.find("\">", endroms);
		initderoms = initderoms + 2;
		endderoms = sourceRoms.find("</a>", initderoms);
		//std::cout << baserom << sourceRoms.substr(initroms,endroms - initroms) << std::endl;
		if (sourceRoms.substr(initroms, endroms - initroms) != ("../"))
		{
			
			std::string completenamerom = sourceRoms.substr(initderoms, endderoms - initderoms);
			replace(completenamerom, "&gt;", "");
			replace(completenamerom, "&amp;", "");
			std::string completeurlrom = baserom + sourceRoms.substr(initroms, endroms - initroms);
			
			if(case_insensitive_match(completenamerom,filtred))
			{
				arraymain.push_back(completenamerom);
				arraychapter.push_back(completeurlrom);
				arraynamedown.push_back(sourceRoms.substr(initroms, endroms - initroms));
			}
		}
		initroms++;
}}

int reloadromfiltreddata(void* data)
{
	
	std::string sourceRoms = gethtml(baserom);
	int initcode = sourceRoms.find("<h1>Index of /public/rom/");
	int endcode = sourceRoms.find("</body>", initcode);
	int initroms = 0;
	int endroms;
	int initderoms, endderoms;
	sourceRoms = sourceRoms.substr(initcode, endcode - initcode);
	loading = false;
	while (initroms != -1) {
		initroms = sourceRoms.find("<a href=\"", initroms);
		if (initroms == -1) { break; }
		initroms = initroms + 9;
		endroms = sourceRoms.find("\"", initroms);
		initderoms = sourceRoms.find("\">", endroms);
		initderoms = initderoms + 2;
		endderoms = sourceRoms.find("</a>", initderoms);
		//std::cout << baserom << sourceRoms.substr(initroms,endroms - initroms) << std::endl;
		if (sourceRoms.substr(initroms, endroms - initroms) != ("../"))
		{

			std::string completenamerom = sourceRoms.substr(initderoms, endderoms - initderoms);
			replace(completenamerom, "&gt;", "");
			replace(completenamerom, "&amp;", "");
			std::string completeurlrom = baserom + sourceRoms.substr(initroms, endroms - initroms);

			if (case_insensitive_match(completenamerom, tosearch))
			{
				arraymain.push_back(completenamerom);
				arraychapter.push_back(completeurlrom);
				arraynamedown.push_back(sourceRoms.substr(initroms, endroms - initroms));
			}
		}
		initroms++;
	}
	return 0;
}
int main(int argc, char **argv)

{
#ifdef __SWITCH__
	romfsInit();
	socketInitializeDefault();
	nxlinkStdio();
	struct stat st = { 0 };

	if (stat("sdmc:/RomsXShop", &st) == -1) {
		mkdir("sdmc:/RomsXShop", 0777);
	}
	if (stat("sdmc:/RomsXShop/SNES", &st) == -1) {
		mkdir("sdmc:/RomsXShop/SNES", 0777);
	}
	if (stat("sdmc:/RomsXShop/NES", &st) == -1) {
		mkdir("sdmc:/RomsXShop/NES", 0777);
	}
	if (stat("sdmc:/RomsXShop/GBA", &st) == -1) {
		mkdir("sdmc:/RomsXShop/GBA", 0777);
	}
	if (stat("sdmc:/RomsXShop/GBC", &st) == -1) {
		mkdir("sdmc:/RomsXShop/GBC", 0777);
	}
	if (stat("sdmc:/RomsXShop/GB", &st) == -1) {
		mkdir("sdmc:/RomsXShop/GB", 0777);
	}
	if (stat("sdmc:/RomsXShop/GENESIS", &st) == -1) {
		mkdir("sdmc:/RomsXShop/GENESIS", 0777);
	}
	if (stat("sdmc:/RomsXShop/PSXNTSC", &st) == -1) {
		mkdir("sdmc:/RomsXShop/PSXNTSC", 0777);
	}
	if (stat("sdmc:/RomsXShop/PSXNTSCJ", &st) == -1) {
		mkdir("sdmc:/RomsXShop/PSXNTSCJ", 0777);
	}
	if (stat("sdmc:/RomsXShop/PSXPAL", &st) == -1) {
		mkdir("sdmc:/RomsXShop/PSXPAL", 0777);
	}
	if (stat("sdmc:/RomsXShop/N64", &st) == -1) {
		mkdir("sdmc:/RomsXShop/N64", 0777);
	}
	if (stat("sdmc:/RomsXShop/PCENGINE", &st) == -1) {
		mkdir("sdmc:/RomsXShop/PCENGINE", 0777);
	}
	if (stat("sdmc:/RomsXShop/NEOGEO", &st) == -1) {
		mkdir("sdmc:/RomsXShop/NEOGEO", 0777);
	}
	if (stat("sdmc:/RomsXShop/SEGAMASTERSYSTEM", &st) == -1) {
		mkdir("sdmc:/RomsXShop/SEGAMASTERSYSTEM", 0777);
	}
	if (stat("sdmc:/RomsXShop/DREAMCAST", &st) == -1) {
		mkdir("sdmc:/RomsXShop/DREAMCAST", 0777);
	}
	if (stat("sdmc:/RomsXShop/COMMODORE64", &st) == -1) {
		mkdir("sdmc:/RomsXShop/COMMODORE64", 0777);
	}
#endif 
	int basex = 0;
	int basey = 0;

	
	arrayconsole.clear();
	arrayconsole.push_back("Super Nintendo");
	arrayconsole.push_back("GameBoy Advance");
	arrayconsole.push_back("GameBoy Color");
	arrayconsole.push_back("GameBoy");
	arrayconsole.push_back("Nintendo NES");
	arrayconsole.push_back("Sega Genesis");
	arrayconsole.push_back("PlayStation NTSC");
	arrayconsole.push_back("PlayStation NTSC-J");
	arrayconsole.push_back("PlayStation PAL");
	arrayconsole.push_back("Nintendo 64");
	arrayconsole.push_back("PC ENGINE");
	arrayconsole.push_back("Neo Geo");
	arrayconsole.push_back("Sega Master System");
	arrayconsole.push_back("DreamCast");
	arrayconsole.push_back("Commodore 64");
	//Start up SDL and create window
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
#ifdef __SWITCH__
		gWindow = SDL_CreateWindow("sdl2_gles2", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
#else
		gWindow = SDL_CreateWindow("RomsXShop", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
#endif // SWITCH

		
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());

		}
		else
		{
			//Create vsynced renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());

				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());

				}
			}
		}
	}



	

#ifdef __SWITCH__

	gFont = TTF_OpenFont("romfs:/lazy.ttf", 16);
	gFont2 = TTF_OpenFont("romfs:/lazy2.ttf", 30);
	gFontcapit = TTF_OpenFont("romfs:/lazy2.ttf", 100);
	gFont3 = TTF_OpenFont("romfs:/lazy2.ttf", 40);

	
	lupa.loadFromFile("romfs:/lupa.png");
	Heart.loadFromFile("romfs:/heart.png");
	Farest.loadFromFile("romfs:/texture.png");
	//std::string foldercomic = "sdmc:/Kaguya Wants to be Confessed/";
	std::string foldermain = "sdmc:/RomsXShop/";
	std::string folderSnes = "sdmc:/RomsXShop/SNES";
	std::string folderNes = "sdmc:/RomsXShop/NES";
#else
	gFont = TTF_OpenFont("C:\\respaldo2017\\C++\\RomsXShop\\Debug\\lazy.ttf", 16);
	gFont2 = TTF_OpenFont("C:\\respaldo2017\\C++\\RomsXShop\\Debug\\lazy2.ttf", 30);
	gFontcapit = TTF_OpenFont("C:\\respaldo2017\\C++\\RomsXShop\\Debug\\lazy2.ttf", 100);
	gFont3 = TTF_OpenFont("C:\\respaldo2017\\C++\\RomsXShop\\Debug\\lazy2.ttf", 40);
	Heart.loadFromFile("C:/respaldo2017/C++/RomsXShop/Debug/heart.png");
	Farest.loadFromFile("C:/respaldo2017/C++/RomsXShop/Debug/texture.png");
	lupa.loadFromFile("C:/respaldo2017/C++/RomsXShop/Debug/lupa.png");
	//std::string foldercomic = "C:/respaldo2017/C++/RomsXShop/Debug/Kaguya Wants to be Confessed/";
	std::string foldermain = "C:/respaldo2017/C++/RomsXShop/Debug/";
	std::string folderSnes = "C:/respaldo2017/C++/RomsXShop/Debug/SNES";
	std::string folderNes = "C:/respaldo2017/C++/RomsXShop/Debug/NES";
#endif // SWITCH

	SDL_Thread* threadID = NULL;
	SDL_Thread* threadID2 = NULL;
	SDL_Thread* threadID3 = NULL;
	//reloadrom();

	
	


	bool cascade = false;
	int cascadesize = 0;
	bool separation = false;
	bool adjust = true;
	int speed = 15;
	int zoom = 1;
	SDL_Color textColor = { 50, 50, 50 };

	
	//Main loop flag
	int quit = 0;

	//Event handler
	SDL_Event e;
#ifdef __SWITCH__
	for (int i = 0; i < 2; i++) {
		if (SDL_JoystickOpen(i) == NULL) {
			SDL_Log("SDL_JoystickOpen: %s\n", SDL_GetError());
			SDL_Quit();
			return -1;
		}
	}
#endif // SWITCH

	
	//While application is running
#ifdef __SWITCH__
	while ( appletMainLoop() && !quit)
#else
	while (!quit )
#endif // SWITCH
	
	{
		
		
#ifdef __SWITCH__
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();

		//hidKeysDown returns information about which buttons have been just pressed (and they weren't in the previous frame)
		u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		//hidKeysHeld returns information about which buttons have are held down in this frame
		u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
		//hidKeysUp returns information about which buttons have been just released
		u64 kUp = hidKeysUp(CONTROLLER_P1_AUTO);

		if (kDown & KEY_PLUS)
		{
			//quit = 1;
			
		}
		if (kHeld & KEY_LSTICK_DOWN)
		{
			switch (statenow)
			{
			case readmanga:

				if (cascade == false) {

					if ((basey) > (SCREEN_HEIGHT - heightnew))
					{
						basey -= speed;
						if (basey <= SCREEN_HEIGHT - heightnew)
						{
							basey = SCREEN_HEIGHT - heightnew;
						}
					}
				}
				else
				{
					basey -= speed;
				}
				break;

			case selectmanga:
				if (selectchapter < arraymain.size() - 1)
				{
					selectchapter++;

					std::cout << selectchapter << std::endl;
					baseymain = baseymain - 20;
				}
				else {
					selectchapter = 0;

					baseymain = 60;
				}
				break;

			case selectconsole:
				if (searchchapter < arrayconsole.size() - 1)
				{
					searchchapter++;

					std::cout << searchchapter << std::endl;
					baseymain = baseymain - 20;
				}
				else {
					searchchapter = 0;

					baseymain = 60;
				}
				break;
			}


		}
		if (kDown & KEY_LSTICK_DOWN)
		{
		

		}
		if (kHeld & KEY_LSTICK_UP)
		{
			switch (statenow)
			{
			case readmanga:

				if (cascade == false) {
					if (basey <= 0)
					{
						basey += speed;
						if (basey > 0)
						{
							basey = 0;
						}
					}
				}
				else
				{
					if (basey <= 0)
					{
						basey += speed;
						if (basey > 0)
						{
							basey = 0;
						}
					}
				}
				break;
			case selectmanga:
				if (selectchapter > 0)
				{
					selectchapter--;
					std::cout << selectchapter << std::endl;
					baseymain = baseymain + 20;
				}
				else {
					baseymain = 60 - 20 * (arraymain.size() - 1);
					selectchapter = arraymain.size() - 1;
				}
				break;

			case selectconsole:
				if (searchchapter > 0)
				{
					searchchapter--;
					std::cout << searchchapter << std::endl;
					baseymain = baseymain + 20;
				}
				else {
					baseymain = 60 - 20 * (arrayconsole.size() - 1);
					searchchapter = arrayconsole.size() - 1;
				}
				break;
			}

		}
		if (kDown & KEY_LSTICK_UP)
		{
			

		}
		if (kHeld & KEY_LSTICK_LEFT)
		{

			

		}

		if (kHeld & KEY_LSTICK_RIGHT)
		{
		

		}
		if (kDown & KEY_DUP)
		{
			switch (statenow)
			{
			case readmanga:

				if (cascade == false) {
					if (basey <= 0)
					{
						basey += speed;
						if (basey > 0)
						{
							basey = 0;
						}
					}
				}
				else
				{
					if (basey <= 0)
					{
						basey += speed;
						if (basey > 0)
						{
							basey = 0;
						}
					}
				}
				break;
			case selectmanga:
				if (selectchapter > 0)
				{
					selectchapter--;
					std::cout << selectchapter << std::endl;
					baseymain = baseymain + 20;
				}
				else {
					baseymain = 60 - 20 * (arraymain.size() - 1);
					selectchapter = arraymain.size() - 1;
				}
				break;

			case selectconsole:
				if (searchchapter > 0)
				{
					searchchapter--;
					std::cout << searchchapter << std::endl;
					baseymain = baseymain + 20;
				}
				else {
					baseymain = 60 - 20 * (arrayconsole.size() - 1);
					searchchapter = arrayconsole.size() - 1;
				}
				break;
			}

		}
		if (kDown & KEY_DDOWN)
		{

			switch (statenow)
			{
			case readmanga:

				if (cascade == false) {

					if ((basey) > (SCREEN_HEIGHT - heightnew))
					{
						basey -= speed;
						if (basey <= SCREEN_HEIGHT - heightnew)
						{
							basey = SCREEN_HEIGHT - heightnew;
						}
					}
				}
				else
				{
					basey -= speed;
				}
				break;

			case selectmanga:
				if (selectchapter < arraymain.size() - 1)
				{
					selectchapter++;

					std::cout << selectchapter << std::endl;
					baseymain = baseymain - 20;
				}
				else {
					selectchapter = 0;

					baseymain = 60;
				}
				break;

			case selectconsole:
				if (searchchapter < arrayconsole.size() - 1)
				{
					searchchapter++;

					std::cout << searchchapter << std::endl;
					baseymain = baseymain - 20;
				}
				else {
					searchchapter = 0;

					baseymain = 60;
				}
				break;
			}


		}
		if (kHeld & KEY_ZL)
		{
			
		}

		if (kHeld & KEY_ZR)
		{
		
		}

		if (kDown & KEY_L)
		{
		}

		if (kDown & KEY_R)
		{
			switch (statenow)
			{
			case selectmanga:
				
				char *buf = (char*)malloc(256);
#ifdef __SWITCH__
				strcpy(buf, Keyboard_GetText("ROM Name (example: mario)", ""));
#endif // SWITCH


				std::string tempbus(buf);
				tosearch = tempbus;
				if (tosearch != "")
				{
					selectchapter = 0;

					baseymain = 60;
					loading = true;
					arraymain.clear();
					arraychapter.clear();
					arraynamedown.clear();
					threadID3 = SDL_CreateThread(reloadromfiltreddata, "jkthread3", (void*)NULL);
				}
				//reloadromfiltred(tempbus);

				break;
			}
			

		}

		if (kDown & KEY_RSTICK)
		{
		
		}

		if (kDown & KEY_A)
		{
			switch (statenow)
			{
			case readmanga:


				break;
			case selectmanga:
				statenow = readmanga;
				fulldownloaded = false;
				threadID = SDL_CreateThread(downloaddata, "jkthread", (void*)NULL);
				break;

			case selectconsole:
				loading = true;
				arraymain.clear();
				arraychapter.clear();
				arraynamedown.clear();
				statenow = selectmanga;
				selectchapter = 0;

				baseymain = 60;
				switch (searchchapter)
				{
				case snesrom:

					typerom = "Super Nintendo";
					baserom = "https://the-eye.eu/public/rom/SNES/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/SNES/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/SNES/";

#endif // SWITCH
					break;
				case gbarom:

					typerom = "GameBoy Advance";
					baserom = "https://the-eye.eu/public/rom/Nintendo%20Gameboy%20Advance/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/GBA/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GBA/";

#endif // SWITCH
					break;
				case gbcrom:


					typerom = "GameBoy Color";
					baserom = "https://the-eye.eu/public/rom/Nintendo%20Gameboy%20Color/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/GBC/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GBC/";

#endif // SWITCH


					break;

				case gbrom:


					typerom = "GameBoy";
					baserom = "https://the-eye.eu/public/rom/Nintendo%20Gameboy/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/GB/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GB/";

#endif // SWITCH


					break;

				case nesrom:


					typerom = "Nintendo Entertainment System";
					baserom = "https://the-eye.eu/public/rom/NES/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/NES/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/NES/";

#endif // SWITCH


					break;

				case genesisrom:


					typerom = "Sega Genesis";
					baserom = "https://the-eye.eu/public/rom/Sega%20Genesis/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/GENESIS/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GENESIS/";

#endif // SWITCH


					break;

				case psxntsc:


					typerom = "PlayStation NTSC";
					baserom = "https://the-eye.eu/public/rom/Playstation/Games/NTSC/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/PSXNTSC/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PSXNTSC/";

#endif // SWITCH


					break;

				case psxntscj:


					typerom = "PlayStation NTSC-J";
					baserom = "https://the-eye.eu/public/rom/Playstation/Games/NTSC-J/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/PSXNTSCJ/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PSXNTSCJ/";

#endif // SWITCH


					break;

				case psxpal:


					typerom = "PlayStation PAL";
					baserom = "https://the-eye.eu/public/rom/Playstation/Games/PAL/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/PSXPAL/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PSXPAL/";

#endif // SWITCH


					break;


				case n64rom:


					typerom = "Nintendo 64";
					baserom = "https://the-eye.eu/public/rom/Nintendo%2064/Roms/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/N64/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/N64/";

#endif // SWITCH


					break;

				case pcenginerom:


					typerom = "PCEngine";
					baserom = "https://the-eye.eu/public/rom/NEC%20PC%20Engine%20TurboGrafx%2016/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/PCENGINE/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PCENGINE/";

#endif // SWITCH


					break;

				case neogeorom:


					typerom = "Neo Geo";
					baserom = "https://the-eye.eu/public/rom/SNK%20Neo%20Geo/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/NEOGEO/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/NEOGEO/";

#endif // SWITCH


					break;

				case mastersystemrom:


					typerom = "Sega Master System";
					baserom = "https://the-eye.eu/public/rom/Sega%20Master%20System/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/SEGAMASTERSYSTEM/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/SEGAMASTERSYSTEM/";

#endif // SWITCH


					break;

				case dreamcastrom:


					typerom = "DreamCast";
					baserom = "https://the-eye.eu/public/rom/Sega%20Dreamcast/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/DREAMCAST/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/DREAMCAST/";

#endif // SWITCH


					break;

				case commodore64rom:


					typerom = "Commodore 64";
					baserom = "https://the-eye.eu/public/rom/Commodore%2064/";

#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/COMMODORE64/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/COMMODORE64/";

#endif // SWITCH
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);

					break;
						}

				break;

					}


		}

		if (kDown & KEY_X)
		{
			
		}

		if (kDown & KEY_LSTICK)
		{
			
		}
		
		if (kDown & KEY_B)
		{
			switch (statenow)
			{
			case readmanga:
				statenow = selectmanga;


				break;
			case selectmanga:
				statenow = selectconsole;
				searchchapter = 0;


				baseymain = 60;
				break;
					}
		}

#endif


		//Handle events on queue
		while (SDL_PollEvent(&e))
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				cancelcurl = 1;
				quit = 1;
			}
			//User presses a key
			else if (e.type == SDL_KEYDOWN)
			{
				//Select surfaces based on key press
				switch (e.key.keysym.sym)
				{
				case SDLK_DOWN:

					switch (statenow)
					{ case readmanga:
						
						if (cascade == false) {

							if ((basey) > (SCREEN_HEIGHT - heightnew))
							{
								basey -= speed;
								if (basey <= SCREEN_HEIGHT - heightnew)
								{
									basey = SCREEN_HEIGHT - heightnew;
								}
							}
						}
						else
						{
							basey -= speed;
						}
						break;

					case selectmanga:
						if (selectchapter < arraymain.size() - 1)
						{
							selectchapter++;

							std::cout << selectchapter << std::endl;
							baseymain = baseymain - 20;
						}
						else {
							selectchapter = 0;
							
							 baseymain = 60;
						}
						break;

					case selectconsole:
						if (searchchapter < arrayconsole.size() - 1)
						{
							searchchapter++;

							std::cout << searchchapter << std::endl;
							baseymain = baseymain - 20;
						}
						else {
							searchchapter = 0;

							baseymain = 60;
						}
						break;
					}
					
					break;

				case SDLK_UP:
					switch (statenow)
					{
					case readmanga:

						if (cascade == false) {
							if (basey <= 0)
							{
								basey += speed;
								if (basey > 0)
								{
									basey = 0;
								}
							}
						}
						else
						{
							if (basey <= 0)
							{
								basey += speed;
								if (basey > 0)
								{
									basey = 0;
								}
							}
						}
						break;
					case selectmanga:
						if (selectchapter > 0)
						{
							selectchapter--;
							std::cout << selectchapter << std::endl;
							baseymain =  baseymain + 20;
						}
						else {
							baseymain =  60 -20 * (arraymain.size() - 1) ;
							selectchapter = arraymain.size() - 1;
						}
						break;

					case selectconsole:
						if (searchchapter > 0)
						{
							searchchapter--;
							std::cout << searchchapter << std::endl;
							baseymain = baseymain + 20;
						}
						else {
							baseymain = 60 - 20 * (arrayconsole.size() - 1);
							searchchapter = arrayconsole.size() - 1;
						}
						break;
					}
					break;
				case SDLK_a:
					switch (statenow)
					{
					case readmanga:
						
						
						break;
					case selectmanga:
						statenow = readmanga;
						fulldownloaded = false;
						threadID = SDL_CreateThread(downloaddata, "jkthread", (void*)NULL);
						break;
					
					case selectconsole:
						loading = true;
						arraymain.clear();
						arraychapter.clear();
						arraynamedown.clear();
						statenow = selectmanga;
						selectchapter = 0;

						baseymain = 60;
						switch (searchchapter)
						{
						case snesrom:

							typerom = "Super Nintendo";
							baserom = "https://the-eye.eu/public/rom/SNES/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/SNES/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/SNES/";

#endif // SWITCH
							break;
						case gbarom:
							
							typerom = "GameBoy Advance";
							baserom = "https://the-eye.eu/public/rom/Nintendo%20Gameboy%20Advance/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/GBA/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GBA/";

#endif // SWITCH
							break;
						case gbcrom:

							
							typerom = "GameBoy Color";
							baserom = "https://the-eye.eu/public/rom/Nintendo%20Gameboy%20Color/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/GBC/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GBC/";

#endif // SWITCH


							break;

						case gbrom:

							
							typerom = "GameBoy";
							baserom = "https://the-eye.eu/public/rom/Nintendo%20Gameboy/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/GB/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GB/";

#endif // SWITCH


							break;

						case nesrom:

							
							typerom = "Nintendo Entertainment System";
							baserom = "https://the-eye.eu/public/rom/NES/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/NES/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/NES/";

#endif // SWITCH


							break;

						case genesisrom:

							
							typerom = "Sega Genesis";
							baserom = "https://the-eye.eu/public/rom/Sega%20Genesis/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/GENESIS/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/GENESIS/";

#endif // SWITCH


							break;

						case psxntsc:

							
							typerom = "PlayStation NTSC";
							baserom = "https://the-eye.eu/public/rom/Playstation/Games/NTSC/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/PSXNTSC/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PSXNTSC/";

#endif // SWITCH


							break;

						case psxntscj:


							typerom = "PlayStation NTSC-J";
							baserom = "https://the-eye.eu/public/rom/Playstation/Games/NTSC-J/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/PSXNTSCJ/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PSXNTSCJ/";

#endif // SWITCH


							break;

						case psxpal:

							
							typerom = "PlayStation PAL";
							baserom = "https://the-eye.eu/public/rom/Playstation/Games/PAL/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/PSXPAL/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PSXPAL/";

#endif // SWITCH


							break;


						case n64rom:


							typerom = "Nintendo 64";
							baserom = "https://the-eye.eu/public/rom/Nintendo%2064/Roms/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/N64/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/N64/";

#endif // SWITCH


							break;

						case pcenginerom:


							typerom = "PCEngine";
							baserom = "https://the-eye.eu/public/rom/NEC%20PC%20Engine%20TurboGrafx%2016/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/PCENGINE/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/PCENGINE/";

#endif // SWITCH


							break;

						case neogeorom:


							typerom = "Neo Geo";
							baserom = "https://the-eye.eu/public/rom/SNK%20Neo%20Geo/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/NEOGEO/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/NEOGEO/";

#endif // SWITCH


							break;

						case mastersystemrom:


							typerom = "Sega Master System";
							baserom = "https://the-eye.eu/public/rom/Sega%20Master%20System/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/SEGAMASTERSYSTEM/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/SEGAMASTERSYSTEM/";

#endif // SWITCH


							break;

						case dreamcastrom:


							typerom = "DreamCast";
							baserom = "https://the-eye.eu/public/rom/Sega%20Dreamcast/";
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/DREAMCAST/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/DREAMCAST/";

#endif // SWITCH


							break;

						case commodore64rom:


							typerom = "Commodore 64";
							baserom = "https://the-eye.eu/public/rom/Commodore%2064/";
							
#ifdef __SWITCH__
							directoryrom = "sdmc:/RomsXShop/COMMODORE64/";

#else
							directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/COMMODORE64/";

#endif // SWITCH
							threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);

							break;
						}
						
						break;
					
					}



					
				
					break;

				
				case SDLK_MINUS:
					
					
					break;
				case SDLK_b:
					switch (statenow)
					{case readmanga:
						statenow = selectmanga;
						

						break;
					case selectmanga:
						statenow = selectconsole;
						searchchapter = 0;
						

						baseymain = 60;
						break;
					}
					
					break;

				case SDLK_l:
					switch (statenow)
					{
					case selectconsole:
						loading = true;
						arraymain.clear();
					arraychapter.clear();
					arraynamedown.clear();
					statenow = selectmanga;
					selectchapter = 0;

					baseymain = 60;
					typerom = "Super Nintendo";
					baserom = "https://the-eye.eu/public/rom/SNES/";
					threadID2 = SDL_CreateThread(reloadromdata, "jkthread2", (void*)NULL);
#ifdef __SWITCH__
					directoryrom = "sdmc:/RomsXShop/SNES/";

#else
					directoryrom = "C:/respaldo2017/C++/RomsXShop/Debug/SNES/";

#endif // SWITCH
						break;
					}
					
				break;

				case SDLK_r:
					

					break;

				case SDLK_z:
					

					break;
				case SDLK_x:
					switch (statenow)
					{
					
					}
					break;
				case SDLK_w:
					switch (statenow)
					{
					
					}
					break;

				case SDLK_LEFT:
					

				break;



				case SDLK_RIGHT:
					

					break;


				case SDLK_m:
					switch (statenow)
					{
					case readmanga:
						
							neutralsize = neutralsize + 0.05;
						
						break;
					}
					break;

				case SDLK_n:
					switch (statenow)
					{
					case readmanga:
						if (neutralsize > 0.05) {
							neutralsize = neutralsize - 0.05;
						}
						break;
					}
					break;

				default:

					break;
				}
			}



		}









		//Clear screen
		SDL_SetRenderDrawColor(gRenderer, 233, 234, 237, 0xFF);
		SDL_RenderClear(gRenderer);

		//Render current frame
		//wallpaper
		
		
		Farest.render((0), (0));
		int ycascade = 0;
		switch (statenow)
		{
		case selectconsole:

			for (int x = 0; x < arrayconsole.size(); x++) {









				if (x == searchchapter) {
					gTextTexture.loadFromRenderedText(gFont, arrayconsole[x], { 200, 0, 0 });
					gTextTexture.render(basexmain + 42, baseymain + (x * 20));

				}
				else
				{

					gTextTexture.loadFromRenderedText(gFont, arrayconsole[x], { 50, 50, 50 });
					gTextTexture.render(basexmain, baseymain + (x * 20));

				}

				if (x == searchchapter) {
					Heart.render(basexmain, baseymain - 2 + (x * 20));

				}

			}

			//abajo
		{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

		SDL_RenderFillRect(gRenderer, &fillRect); }
		//arriba
		{SDL_Rect fillRect = { 0,  0, 1280, 52 };
		SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

		SDL_RenderFillRect(gRenderer, &fillRect); }

		textColor = { 50, 50, 50 };

		gTextTexture.loadFromRenderedText(gFont, "\"A\" to select console.", textColor);
		gTextTexture.render(basexmain, SCREEN_HEIGHT - 30);
		//gTextTexture.loadFromRenderedText(gFont, "If it freezes, restart.", textColor);
		//gTextTexture.render(SCREEN_WIDTH - 380, SCREEN_HEIGHT - 60);
		gTextTexture.loadFromRenderedText(gFont, "#RenunciaPiñera", textColor);
		gTextTexture.render(SCREEN_WIDTH - 270, SCREEN_HEIGHT - 35);
		gTextTexture.loadFromRenderedText(gFont2, "RomsXShop 1.2.1 (Final)", textColor);

		gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 10);

			
			
			break;

		case readmanga:
			gTextTexture.loadFromRenderedText(gFont3, std::to_string(porcendown) + "\%", textColor);
			gTextTexture.render( SCREEN_WIDTH/2 - gTextTexture.getWidth()/2, SCREEN_HEIGHT/2);
			gTextTexture.loadFromRenderedText(gFont, "Estimated Size: " + std::to_string((int)(sizeestimated/1000 )) + "kb.", textColor);
			gTextTexture.render( 30, 60);
			gTextTexture.loadFromRenderedText(gFont, "Name: " + urlDecode(arraynamedown[selectchapter]), {130,0,0});
			gTextTexture.render(30, 90);

			
			
			

			if (std::to_string(porcendown) == "100") {



			



				//Render red filled quad
				SDL_Rect fillRect = {  0,  400, 1280, 50 };
				SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
				SDL_RenderFillRect(gRenderer, &fillRect);
				gTextTexture.loadFromRenderedText(gFont2, "Download Complete! Check your SD.", textColor);
				gTextTexture.render( 100,  410);
			}
			//abajo
			{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(gRenderer, &fillRect); }
			//arriba
			{SDL_Rect fillRect = { 0,  0, 1280, 52 };
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(gRenderer, &fillRect); }

			textColor = { 50, 50, 50 };

			gTextTexture.loadFromRenderedText(gFont, "\"B\" to go back. ", textColor);
			gTextTexture.render(basexmain, SCREEN_HEIGHT - 30);
			gTextTexture.loadFromRenderedText(gFont, "#RenunciaPiñera", textColor);
			gTextTexture.render(SCREEN_WIDTH - 270, SCREEN_HEIGHT - 60);
			gTextTexture.loadFromRenderedText(gFont2, typerom, textColor);

			gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 10);
		
		
		break;


		case selectmanga:
			
			
			
			
			if (existfoldermain == false)
			{
				gTextTexture.loadFromRenderedText(gFont3, "Please create the folder called \"RomsXShop\" in", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 100);
				gTextTexture.loadFromRenderedText(gFont3, "your SD and put your \"Roms\" inside", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 130);
				
					gTextTexture.loadFromRenderedText(gFont3, "in a folder without Latin characters.", { 0, 0, 0 });
				gTextTexture.render(SCREEN_WIDTH / 2 - gTextTexture.getWidth() / 2, 160);
				
			}
			int cantidadtemp = 0;
			if (selectchapter + 30 >= arraymain.size())
			{
				cantidadtemp = arraymain.size();
			}
			else
			{
				cantidadtemp = selectchapter + 30;
			}
			for (int x = selectchapter; x <cantidadtemp; x++) {

				
				
				



				

				if (x == selectchapter) {
					gTextTexture.loadFromRenderedText(gFont, arraymain[x], { 200, 0, 0 });
					gTextTexture.render(basexmain + 42, baseymain +(x * 20));

				}
				else
				{

					gTextTexture.loadFromRenderedText(gFont, arraymain[x], { 50, 50, 50 });
					gTextTexture.render(basexmain, baseymain + (x * 20));

				}

				if (x == selectchapter) {
					Heart.render(basexmain, baseymain - 2 + (x * 20));
			
				}

			}
			//abajo
			{SDL_Rect fillRect = { 0, SCREEN_HEIGHT - 35, 1280, 25 };
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(gRenderer, &fillRect); }
			//arriba
			{SDL_Rect fillRect = { 0,  0, 1280, 52 };
			SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

			SDL_RenderFillRect(gRenderer, &fillRect); }

			textColor = { 50, 50, 50 };
			
				gTextTexture.loadFromRenderedText(gFont, "\"A\" to download the Rom. - \"R\" to search the Rom.  ", textColor);
				gTextTexture.render(basexmain, SCREEN_HEIGHT - 30);
				gTextTexture.loadFromRenderedText(gFont, "#RenunciaPiñera", textColor);
				gTextTexture.render(SCREEN_WIDTH - 270, SCREEN_HEIGHT - 60);
				gTextTexture.loadFromRenderedText(gFont2, typerom, textColor);
				
				gTextTexture.render(SCREEN_WIDTH/2 - gTextTexture.getWidth()/2 , 10);

				if (loading == true) {
					{SDL_Rect fillRect = { 0,  SCREEN_HEIGHT / 2 - 26, 1280, 52 };
					SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

					SDL_RenderFillRect(gRenderer, &fillRect); }
					gTextTexture2.loadFromRenderedText(gFont2, "Cargando, espere por favor...", textColor);

					gTextTexture2.render(SCREEN_WIDTH / 2 - gTextTexture2.getWidth() / 2, SCREEN_HEIGHT / 2 -gTextTexture2.getHeight()/2);
				}

			break;
		


		}



		//Update screen
		SDL_RenderPresent(gRenderer);


	}
	
	if (NULL == threadID) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(threadID, NULL);

	}

	if (NULL == threadID2) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(threadID2, NULL);

	}
	if (NULL == threadID3) {
		printf("SDL_CreateThread failed: %s\n", SDL_GetError());
	}
	else {
		SDL_WaitThread(threadID3, NULL);

	}
	//Free resources and close SDL
#ifdef __SWITCH__
	socketExit();
	romfsExit();
#endif // SWITCH


	
	close();

	return 0;

}
