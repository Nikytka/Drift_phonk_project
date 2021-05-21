#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Audio.hpp>

#include "client.h"
#include "viewer.h"
#include "controls.h"
#include <math.h>

const float PI = 3.141592;

// Client main function
int main()
{
    // Creating game world
    World world;

    // Creating vector of samples default music
    std::vector<sf::Music> def_samples(5);
    def_samples[0].openFromFile("mus0.ogg");
    def_samples[1].openFromFile("mus1.ogg");
    def_samples[2].openFromFile("mus2.ogg");
    def_samples[3].openFromFile("mus3.ogg");
    def_samples[4].openFromFile("mus4.ogg");

    sf::Music gas_sample;
    gas_sample.openFromFile("gas.ogg");
    gas_sample.setLoop(true);
    gas_sample.setVolume(0);
    gas_sample.play();

    sf::Music idle_sample;
    idle_sample.openFromFile("idle.ogg");
    idle_sample.setLoop(true);
    idle_sample.setVolume(0);
    idle_sample.play();

    // Creating vector of samples phonk music
    std::vector<sf::Music> ph_samples(5);
    ph_samples[0].openFromFile("phonk1.ogg");
    ph_samples[1].openFromFile("phonk2.ogg");
    ph_samples[2].openFromFile("phonk3.ogg");
    ph_samples[3].openFromFile("phonk4.ogg");
    ph_samples[4].openFromFile("phonk5.ogg");

    // Start playing menu default music sample
    def_samples[0].setVolume(30);
    def_samples[0].play();

    // Start playing first phonk music sample
    ph_samples[0].play();
    ph_samples[0].setVolume(0);

    // Creating client (args: ip, port, World)
    Client client("127.0.0.1", 1234, world);

    Viewer viewer("My client"); // Creating viewer
    sf::Clock gameClock; // Game clock

    int i1, j1, i2, j2;

    float acs, angle;

    sf::Vector2f prevControls; // Previous player controls vector

    float prevgearControls = 0; // Previous player gear controls

    bool gear_change = true;

    // Main cycle
    while (client.isRunning() && viewer.isOpen())
    {
        // Connect screen scene
        if (world.GetScene() == Scene::Connect)
        {
            client.events_connect(viewer);
            viewer.draw_connect(world);
        }

        // Lobby scene
        if (world.GetScene() == Scene::Lobby)
        {
            viewer.handleEvents();
            client.events_lobby(viewer);
            viewer.draw_lobby(world);
        }

        // Hero selection scene
        if (world.GetScene() == Scene::CarSelection)
        {
            viewer.handleEvents();
            client.events_car_selection(viewer);
            viewer.draw_car_selection(world, client.id());
        }

        // Gameplay scene
        if (world.GetScene() == Scene::Gameplay)
        {
            client.events_gameplay(viewer);

            idle_sample.setVolume(100);

            // Cheking which default sample is playing
            for (i1 = 0; i1 < 5; i1++)
                if (def_samples[i1].getStatus() == sf::Music::Playing)
                    j1 = i1;
            // Stop menu music sample
            def_samples[0].stop();

            // If current default sample is over, the next starts
            if (def_samples[j1].getStatus() == sf::Music::Stopped) {
                def_samples[(j1 + 1) % 5].setVolume(40);
                def_samples[(j1 + 1) % 5].play();
            }

            // Cheking which phonk sample is playing
            for (i2 = 0; i2 < 5; i2++)
                if (ph_samples[i2].getStatus() == sf::Music::Playing)
                    j2 = i2;

            // If current phonk sample is over, the next starts
            if (ph_samples[j2].getStatus() == sf::Music::Stopped) {
                ph_samples[(j2 + 1) % 5].play();
                ph_samples[(j2 + 1) % 5].setVolume(0);
            }

            viewer.handleEvents(); // Handling events

            sf::Vector2f currControls; // Current player controls vector

            // Setting controls of player
            currControls = controls_wasd();

            // Current player gear controls
            float currgearControls;

            currgearControls = controls_arrows();

            // Changing player's velocity to new velocity
            world.get_players()[client.id()].set_controls(currControls);

            // Changing player's gear controls to new gear controls
            world.get_players()[client.id()].set_gear_controls(currgearControls);

            //Checking if car is drifting now (checking difference between angles of sprite and velocity
            if (sqrt(pow(world.get_players()[client.id()].get_vel().x, 2) + pow(world.get_players()[client.id()].get_vel().y, 2)) >= 50) {
                if (world.get_players()[client.id()].get_vel().y <= 0)
                    acs = acos(-world.get_players()[client.id()].get_vel().x / sqrt(pow(world.get_players()[client.id()].get_vel().x, 2) + pow(world.get_players()[client.id()].get_vel().y, 2))) * 180 / PI;
                else
                    acs = acos(world.get_players()[client.id()].get_vel().x / sqrt(pow(world.get_players()[client.id()].get_vel().x, 2) + pow(world.get_players()[client.id()].get_vel().y, 2))) * 180 / PI + 180;
                if (fmod(world.get_players()[client.id()].get_angle(), 360) >= 0)
                    angle = fmod(world.get_players()[client.id()].get_angle(), 360);
                else 
                    angle = 360.0f - fmod(-world.get_players()[client.id()].get_angle(), 360);

                if (abs(angle - acs) > 30 && abs(angle - acs) < 330 && abs(fmod(angle + 180, 360) - acs) > 30 && abs(fmod(angle + 180, 360) - acs) < 330) {
                    def_samples[j1].setVolume(0);
                    ph_samples[j2].setVolume(80);
                }

                else {
                    def_samples[j1].setVolume(40);
                    ph_samples[j2].setVolume(0);
                }
            }
            else {
                def_samples[j1].setVolume(40);
                ph_samples[j2].setVolume(0);
            }

            if (world.get_players()[client.id()].get_controls().y < 0)
            {
                idle_sample.setVolume(0);
                gas_sample.setVolume(60);
            }

            if (world.get_players()[client.id()].get_controls().y== 0)
            {
                idle_sample.setVolume(60);
                gas_sample.setVolume(0);
            }

            // On any change notify server
            if (currControls != prevControls)
            {
                client.notify_server();
                prevControls = currControls;
            }

            if (currgearControls >=0 && world.get_players()[client.id()].get_gear() != 5 || currgearControls <=0 && world.get_players()[client.id()].get_gear() != -1) {

                if (currgearControls == 0.0f && fabs(prevgearControls) == 1) {
                    gear_change = true;
                    prevgearControls = 0.0f;
                }

                if (currgearControls != prevgearControls && gear_change == true)
                { 
                    prevgearControls = currgearControls;
                    gear_change = false;
                    world.get_players()[client.id()].add_gear(currgearControls);
                    client.change_gear();
                }
            }

            // Updating viewer hud
            viewer.updateHudSpeed(world.get_players()[client.id()].getAbsoluteSpeed());
            viewer.updateHudGear(world.get_players()[client.id()].get_gear());

            // Restarting clock and updating world
            const auto dt = gameClock.restart();
            world.update(dt.asSeconds());

            // Drawing world
            viewer.draw_gameplay(world);
        }

        // Pause scene
        if (world.GetScene() == Scene::Pause)
        {
            viewer.handleEvents();
            client.events_pause(viewer);
            viewer.draw_pause();
        }
    }

    return 0;
}