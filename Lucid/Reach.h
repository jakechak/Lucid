#pragma once

#include "Module.h"
#include <jni.h>

class Reach : public Module
{
public:
	Reach();
	void OnUpdate() override;

private:
	jclass aabb_class;

	jmethodID GetLoadedEntities;
	jmethodID GetEntityBB;

	jmethodID GetPlayerLook;
	jmethodID GetPlayerEyes;

	jmethodID SetPosition;
	jmethodID SetBB;

	jmethodID CreateAABB;

	jfieldID bb_minX;
	jfieldID bb_minY;
	jfieldID bb_minZ;
	jfieldID bb_maxX;
	jfieldID bb_maxY;
	jfieldID bb_maxZ;

	jfieldID entityX_field;
	jfieldID entityY_field;
	jfieldID entityZ_field;

	jfieldID vecX;
	jfieldID vecY;
	jfieldID vecZ;

	jobject entity;
};