// toasterTest.cpp : Source file for your target.
//

#include "ToasterGameEngineCore/toaster.h"
#include "ToasterGameEngineCore/application.h"

#define WIDTH 1920
#define HEIGHT 1080
#define WIN_X 100
#define WIN_Y 100

class Pong : public toast::Game 
{
public:
	toast::config init() override
	{
		return { WIN_X , WIN_Y, WIDTH, HEIGHT, "Pong" };
	}

	bool start() override
	{
		return true;
	}

	bool update(float deltaTime) override
	{
		for (int x = 100; x < WIDTH - 100; ++x)
		{
			for (int y = 100; y < HEIGHT - 100; ++y)
			{
				toast::Application::drawPixel(x, y, 0);
			}
		}


		return true;
	}
};

int main()
{
	Pong pong;
	return toast::tmain(pong);
}
