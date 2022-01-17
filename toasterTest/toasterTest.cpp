// toasterTest.cpp : Source file for your target.
//

#include "ToasterGameEngineCore/toaster.h"
#include "ToasterGameEngineCore/application.h"

class Pong : public toast::Game 
{
public:
	toast::config init() override
	{
		//        x     y  width height title
		return { 100 , 100, 1920, 1080, "Pong" };
	}

	bool start() override
	{
		toast::Application::debugLog("Console message, and saved in file");
		toast::debugWrite("Console message");
		return true;
	}

	bool update(float deltaTime) override
	{
		return true;
	}
};

int main()
{
	Pong pong;
	return toast::tmain(pong);
}
