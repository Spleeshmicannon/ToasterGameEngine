// toasterTest.cpp : Source file for your target.
//

#include "ToasterGameEngineCore/toaster.h"

class Pong : public toast::Game 
{
public:
	toast::config init() override
	{
		return { 100 , 100, 1920, 1080, "Pong" };
	}

	bool start() override
	{
		return true;
	}

	bool update() override
	{
		return false;
	}
};

int main()
{
	Pong pong;
	return toast::tmain(pong);
}
