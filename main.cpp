#include "model.h"
#include "view.h"
#include "controller.h"

int main()
{
    GameModel model;

    initModel(model);
    initView();

    while (updateView(model))
        ;

    freeView();
}

