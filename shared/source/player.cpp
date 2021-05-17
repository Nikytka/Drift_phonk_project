#pragma once

#include "player.h"

void Player::update(float dt)
{
	pos += v * dt;
}

float Player::get_maxspeed()
{
	return this->SPEED;
}

sf::Vector2f Player::get_pos()
{
	return this->pos;
}

void Player::set_pos(sf::Vector2f new_pos)
{
	this->pos.x = new_pos.x;
	this->pos.y = new_pos.y;
}

sf::Vector2f Player::get_vel()
{
	return this->v;
}

float Player::get_rad()
{
	return this->RADIUS;
}

void Player::set_vel(sf::Vector2f vel)
{
	this->v = vel;
}

bool Player::isReady()
{
	return this->ready;
}

sf::Vector2f Player::get_controls()
{
	return this->controls;
}

void Player::set_controls(sf::Vector2f controls)
{
	this->controls = controls;
}

void Player::player_ready()
{
	this->ready = true;
}

void Player::player_not_ready()
{
	this->ready = false;
}

int Player::get_selected_car()
{
	return this->selected_car_id;
}

float Player::get_angle()
{
	return this->car_angle;
}

void Player::add_angle(float sign_x, float sign_y)
{
	if (sign_x > 0)
		this->car_angle += 3;
	else 
		this->car_angle -= 3;
}

void Player::add_angle1(float sign_x, float sign_y)
{
	if (sign_x > 0)
		this->car_angle += 2;
	else
		this->car_angle -= 2;
}

void Player::set_angle(float angle)
{
	this->car_angle = angle;
}

float Player::get_gearbox_controls()
{
	return this->gearbox_controls;
}

void Player::set_gear_controls(float gearbox_controls)
{
	this->gearbox_controls = gearbox_controls;
}

int Player::get_gear()
{
	return this->gear;
}

void Player::add_gear(float gear_controls)
{
	this->gear += gear_controls;
}

void Player::set_gear(int gear)
{
	this->gear = gear;
}