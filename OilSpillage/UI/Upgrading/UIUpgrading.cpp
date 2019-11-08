#include "UIUpgrading.h"
#include "../../Input.h"
#include "../../game.h"

void UIUpgrading::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
	{
		this->itemSelector->changeSelectedIndex(false);
	}
	else if(Input::checkButton(Keys::L_RIGHT, States::PRESSED))
	{
		this->itemSelector->changeSelectedIndex(true);
	}
	else if(Input::checkButton(Keys::L_UP, States::PRESSED))
	{
		this->itemSelector->changeSelectedType(false);
	}
	else if(Input::checkButton(Keys::L_DOWN, States::PRESSED))
	{
		this->itemSelector->changeSelectedType(true);
	}

	this->itemSelector->update(deltaTime);
}

void UIUpgrading::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->itemSelector->draw(false);

	const char* type = "";
	switch (this->itemSelector->getSelectedType())
	{
	case WEAPON:
		type = "Weapons";
		break;
	case GADGET:
		type = "Gadgets";
		break;
	case CHASSI:
		type = "Chassis";
		break;
	case WHEEL:
		type = "Wheels";
		break;
	}

	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), type, Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), Colors::White, 0, Vector2(0, 0), 0.5f);
	this->textBox->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIUpgrading::UIUpgrading()
{
}

UIUpgrading::~UIUpgrading()
{
}

void UIUpgrading::init()
{
	this->itemSelector = std::make_unique<ItemSelector>(Vector2(0, 0));
	this->textBox = std::make_unique<TextBox>("Testing some advanced stuff.\nVery\ncool\nright?", Color(Colors::Red), Vector2(400, 150), ArrowPlacement::TOP, Vector2(500, 400));
}
