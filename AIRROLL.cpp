#include "pch.h"
#include "AIRROLL.h"
#include "bakkesmod/wrappers/GameEvent/TutorialWrapper.h"
#include "bakkesmod/wrappers/GameObject/CarWrapper.h"
#include "bakkesmod/wrappers/GameObject/BallWrapper.h"
#include "bakkesmod/wrappers/canvaswrapper.h"

BAKKESMOD_PLUGIN(AIRROLL, "AIRROLL", "1.0", PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void AIRROLL::onLoad()
{
	_globalCvarManager = cvarManager;
	//LOG("Plugin loaded!");
	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("AIRROLL debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	LOG("the cvar with name: {} changed", cvarName);
	//	LOG("the new value is: {}", newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&AIRROLL::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&AIRROLL::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&AIRROLL::YourPluginMethod, this);

    // Variables globales pour la configuration
    CVarWrapper enableCvar;
    CVarWrapper indicatorSizeCvar;
    CVarWrapper opacityCvar;

    // Couleurs pour les différentes directions
    LinearColor pitchUpColor = LinearColor{ 0, 1, 0, 1 };    // Vert
    LinearColor pitchDownColor = LinearColor{ 1, 0, 0, 1 };  // Rouge
    LinearColor yawLeftColor = LinearColor{ 0, 0, 1, 1 };    // Bleu
    LinearColor yawRightColor = LinearColor{ 1, 1, 0, 1 };   // Jaune
    LinearColor rollLeftColor = LinearColor{ 1, 0, 1, 1 };   // Magenta
    LinearColor rollRightColor = LinearColor{ 0, 1, 1, 1 };  // Cyan

    void AirRollTrainer::onLoad()
    {
        // Enregistrement des variables de configuration
        enableCvar = cvarManager->registerCvar("art_enabled", "1", "Active/désactive Air Roll Trainer", true, true, 0, true, 1);
        indicatorSizeCvar = cvarManager->registerCvar("art_size", "30.0", "Taille des indicateurs visuels", true, true, 10.0, true, 100.0);
        opacityCvar = cvarManager->registerCvar("art_opacity", "0.7", "Opacité des indicateurs", true, true, 0.1, true, 1.0);

        // Ajout des hooks
        gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", std::bind(&AirRollTrainer::OnPreProcessInput, this));
        gameWrapper->RegisterDrawable(std::bind(&AirRollTrainer::Render, this, std::placeholders::_1));

        // Commandes pour activer/désactiver et modifier les paramètres
        cvarManager->registerNotifier("art_toggle", [this](std::vector<std::string> params) {
            enableCvar.setValue(!enableCvar.getBoolValue());
            cvarManager->log(enableCvar.getBoolValue() ? "Air Roll Trainer activé" : "Air Roll Trainer désactivé");
            }, "Active/désactive Air Roll Trainer", PERMISSION_ALL);

        // Interface de configuration
        initMenuHooks();

        cvarManager->log("Air Roll Trainer v1.0 chargé");
    }

    void AirRollTrainer::onUnload()
    {
        cvarManager->log("Air Roll Trainer déchargé");
    }

    void AirRollTrainer::initMenuHooks()
    {
        cvarManager->registerCvar("art_pitch_up_color_r", "0", "Composante R de la couleur pour pitch up", true, true, 0, true, 1);
        cvarManager->registerCvar("art_pitch_up_color_g", "1", "Composante G de la couleur pour pitch up", true, true, 0, true, 1);
        cvarManager->registerCvar("art_pitch_up_color_b", "0", "Composante B de la couleur pour pitch up", true, true, 0, true, 1);

        cvarManager->registerCvar("art_pitch_down_color_r", "1", "Composante R de la couleur pour pitch down", true, true, 0, true, 1);
        cvarManager->registerCvar("art_pitch_down_color_g", "0", "Composante G de la couleur pour pitch down", true, true, 0, true, 1);
        cvarManager->registerCvar("art_pitch_down_color_b", "0", "Composante B de la couleur pour pitch down", true, true, 0, true, 1);

        // Ajoutez les autres couleurs de manière similaire...
    }

    void AirRollTrainer::OnPreProcessInput()
    {
        if (!enableCvar.getBoolValue() || !gameWrapper->IsInGame()) return;

        // Cette fonction peut être utilisée pour implémenter une logique supplémentaire
        // ou pour préparer des données avant le rendu
    }

    void AirRollTrainer::Render(CanvasWrapper canvas)
    {
        if (!enableCvar.getBoolValue() || !gameWrapper->IsInGame()) return;

        auto tutorialGame = gameWrapper->GetGameEventAsTutorial();
        if (tutorialGame.IsNull()) return;

        auto car = tutorialGame.GetGameCar();
        if (car.IsNull()) return;

        float size = indicatorSizeCvar.getFloatValue();
        float opacity = opacityCvar.getFloatValue();

        // Ajuster l'opacité des couleurs
        auto adjustOpacity = [opacity](LinearColor color) -> LinearColor {
            color.A = opacity;
            return color;
            };

        // Obtenir la rotation de la voiture
        auto rotation = car.GetRotation();

        // Récupérer les vecteurs de direction de la voiture dans l'espace global
        auto forward = RotatorToVector(rotation);
        auto right = Vector(forward.Y, -forward.X, 0).getNormalized();
        auto up = Vector::cross(forward, right).getNormalized();

        // Position à l'écran
        Vector2 center = { canvas.GetSize().X / 2, canvas.GetSize().Y / 2 };

        // Calculs pour les différentes directions dans l'espace de la voiture
        Vector2F drawForward = WorldToScreen(car.GetLocation() + forward * 100, canvas);
        Vector2F drawBackward = WorldToScreen(car.GetLocation() - forward * 100, canvas);
        Vector2F drawRight = WorldToScreen(car.GetLocation() + right * 100, canvas);
        Vector2F drawLeft = WorldToScreen(car.GetLocation() - right * 100, canvas);
        Vector2F drawUp = WorldToScreen(car.GetLocation() + up * 100, canvas);
        Vector2F drawDown = WorldToScreen(car.GetLocation() - up * 100, canvas);

        // Dessiner les indicateurs
        // Pitch (haut/bas)
        canvas.SetColor(adjustOpacity(pitchUpColor));
        canvas.DrawLine(center, Vector2{ drawUp.X, drawUp.Y }, 3.0f);
        canvas.DrawBox(Vector2{ drawUp.X - size / 2, drawUp.Y - size / 2 }, size, size);

        canvas.SetColor(adjustOpacity(pitchDownColor));
        canvas.DrawLine(center, Vector2{ drawDown.X, drawDown.Y }, 3.0f);
        canvas.DrawBox(Vector2{ drawDown.X - size / 2, drawDown.Y - size / 2 }, size, size);

        // Yaw (gauche/droite)
        canvas.SetColor(adjustOpacity(yawLeftColor));
        canvas.DrawLine(center, Vector2{ drawLeft.X, drawLeft.Y }, 3.0f);
        canvas.DrawBox(Vector2{ drawLeft.X - size / 2, drawLeft.Y - size / 2 }, size, size);

        canvas.SetColor(adjustOpacity(yawRightColor));
        canvas.DrawLine(center, Vector2{ drawRight.X, drawRight.Y }, 3.0f);
        canvas.DrawBox(Vector2{ drawRight.X - size / 2, drawRight.Y - size / 2 }, size, size);

        // Roll (rotation gauche/droite)
        // Pour le roll, nous dessinons des indicateurs sur les côtés de l'écran
        float screenWidth = canvas.GetSize().X;
        float screenHeight = canvas.GetSize().Y;

        // Indicateur pour roll gauche
        canvas.SetColor(adjustOpacity(rollLeftColor));
        canvas.DrawBox(Vector2{ 10, screenHeight / 2 - size / 2 }, size, size);
        canvas.DrawString("Roll Gauche", Vector2{ 10 + size + 5, screenHeight / 2 - 10 }, 1.0f, 1.0f);

        // Indicateur pour roll droit
        canvas.SetColor(adjustOpacity(rollRightColor));
        canvas.DrawBox(Vector2{ screenWidth - 10 - size, screenHeight / 2 - size / 2 }, size, size);
        canvas.DrawString("Roll Droit", Vector2{ screenWidth - 10 - size - 100, screenHeight / 2 - 10 }, 1.0f, 1.0f);

        // Afficher une légende
        float legendY = 50;
        float legendX = 20;
        float textOffset = 30;

        canvas.SetColor(adjustOpacity(LinearColor{ 1, 1, 1, 1 })); // Blanc
        canvas.DrawString("Guide Air Roll Trainer:", Vector2{ legendX, legendY }, 1.5f, 1.5f);

        legendY += 30;

        canvas.SetColor(adjustOpacity(pitchUpColor));
        canvas.DrawBox(Vector2{ legendX, legendY }, 20, 20);
        canvas.DrawString("Lever le nez", Vector2{ legendX + textOffset, legendY }, 1.0f, 1.0f);

        legendY += 25;
        canvas.SetColor(adjustOpacity(pitchDownColor));
        canvas.DrawBox(Vector2{ legendX, legendY }, 20, 20);
        canvas.DrawString("Baisser le nez", Vector2{ legendX + textOffset, legendY }, 1.0f, 1.0f);

        legendY += 25;
        canvas.SetColor(adjustOpacity(yawLeftColor));
        canvas.DrawBox(Vector2{ legendX, legendY }, 20, 20);
        canvas.DrawString("Tourner à gauche", Vector2{ legendX + textOffset, legendY }, 1.0f, 1.0f);

        legendY += 25;
        canvas.SetColor(adjustOpacity(yawRightColor));
        canvas.DrawBox(Vector2{ legendX, legendY }, 20, 20);
        canvas.DrawString("Tourner à droite", Vector2{ legendX + textOffset, legendY }, 1.0f, 1.0f);

        legendY += 25;
        canvas.SetColor(adjustOpacity(rollLeftColor));
        canvas.DrawBox(Vector2{ legendX, legendY }, 20, 20);
        canvas.DrawString("Air roll gauche", Vector2{ legendX + textOffset, legendY }, 1.0f, 1.0f);

        legendY += 25;
        canvas.SetColor(adjustOpacity(rollRightColor));
        canvas.DrawBox(Vector2{ legendX, legendY }, 20, 20);
        canvas.DrawString("Air roll droit", Vector2{ legendX + textOffset, legendY }, 1.0f, 1.0f);
    }

    Vector RotatorToVector(Rotator rotation)
    {
        float pitch = rotation.Pitch * 3.1415926f / 32768.0f;
        float yaw = rotation.Yaw * 3.1415926f / 32768.0f;

        float CP = cos(pitch);
        float SP = sin(pitch);
        float CY = cos(yaw);
        float SY = sin(yaw);

        return Vector(CP * CY, CP * SY, SP);
    }

    Vector2F AirRollTrainer::WorldToScreen(Vector location, CanvasWrapper canvas)
    {
        if (gameWrapper->IsNull())
            return { 0, 0 };

        auto camera = gameWrapper->GetCamera();
        if (camera.IsNull())
            return { 0, 0 };

        Vector2 screenSize = canvas.GetSize();
        auto screenShot = camera.ProjectLocation(location, screenSize.X, screenSize.Y);
        return { screenShot.X, screenShot.Y };
    }
}
