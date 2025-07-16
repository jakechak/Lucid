#include "pch.h"
#include "Reach.h"
#include "Minecraft.h"

Reach::Reach() : Module("Reach", 0x30)
{
    aabb_class = Minecraft::env->FindClass("aug");

    GetLoadedEntities = Minecraft::env->GetMethodID(Minecraft::worldc_class, "E", "()Ljava/util/List;");
    GetEntityBB = Minecraft::env->GetMethodID(Minecraft::entity_class, "aR", "()Laug;");

    GetPlayerLook = Minecraft::env->GetMethodID(Minecraft::entity_class, "d", "(F)Laui;");
    GetPlayerEyes = Minecraft::env->GetMethodID(Minecraft::entity_class, "e", "(F)Laui;");

    SetPosition = Minecraft::env->GetMethodID(Minecraft::entity_class, "b", "(DDD)V");
    SetBB = Minecraft::env->GetMethodID(Minecraft::entity_class, "a", "(Laug;)V");

    CreateAABB = Minecraft::env->GetMethodID(aabb_class, "<init>", "(DDDDDD)V");

    bb_minX = Minecraft::env->GetFieldID(aabb_class, "a", "D");
    bb_minY = Minecraft::env->GetFieldID(aabb_class, "b", "D");
    bb_minZ = Minecraft::env->GetFieldID(aabb_class, "c", "D");
    bb_maxX = Minecraft::env->GetFieldID(aabb_class, "d", "D");
    bb_maxY = Minecraft::env->GetFieldID(aabb_class, "e", "D");
    bb_maxZ = Minecraft::env->GetFieldID(aabb_class, "f", "D");

    entityX_field = Minecraft::env->GetFieldID(Minecraft::entity_class, "s", "D");
    entityY_field = Minecraft::env->GetFieldID(Minecraft::entity_class, "t", "D");
    entityZ_field = Minecraft::env->GetFieldID(Minecraft::entity_class, "u", "D");

    vecX = Minecraft::env->GetFieldID(Minecraft::vec_class, "a", "D");
    vecY = Minecraft::env->GetFieldID(Minecraft::vec_class, "b", "D");
    vecZ = Minecraft::env->GetFieldID(Minecraft::vec_class, "c", "D");
}

void Reach::OnUpdate()
{
	if (!this->IsToggled())
		return;

    jobject player = Minecraft::env->GetObjectField(Minecraft::mc_inst, Minecraft::player_field);
    jobject worldc = Minecraft::env->GetObjectField(Minecraft::mc_inst, Minecraft::worldc_field);
    jobject entity_list = Minecraft::env->CallObjectMethod(worldc, GetLoadedEntities);

    jclass list_class = Minecraft::env->GetObjectClass(entity_list);
    jmethodID GetListSize = Minecraft::env->GetMethodID(list_class, "size", "()I");
    jmethodID GetEntityFromList = Minecraft::env->GetMethodID(list_class, "get", "(I)Ljava/lang/Object;");

    jint list_size = Minecraft::env->CallIntMethod(entity_list, GetListSize);

    for (jint i = 0; i < list_size; i++)
    {
        jobject entity = Minecraft::env->CallObjectMethod(entity_list, GetEntityFromList, i);

        if (!Minecraft::env->IsInstanceOf(entity, Minecraft::livingentitybase_class) || Minecraft::env->IsSameObject(entity, player))
            continue;

        jobject entity_bb = Minecraft::env->CallObjectMethod(entity, GetEntityBB);

        double x1 = Minecraft::env->GetDoubleField(entity_bb, bb_minX);
        double y1 = Minecraft::env->GetDoubleField(entity_bb, bb_minY);
        double z1 = Minecraft::env->GetDoubleField(entity_bb, bb_minZ);
        double x2 = Minecraft::env->GetDoubleField(entity_bb, bb_maxX);
        double y2 = Minecraft::env->GetDoubleField(entity_bb, bb_maxY);
        double z2 = Minecraft::env->GetDoubleField(entity_bb, bb_maxZ);

        double entityX = Minecraft::env->GetDoubleField(entity, entityX_field);
        double entityY = Minecraft::env->GetDoubleField(entity, entityY_field);
        double entityZ = Minecraft::env->GetDoubleField(entity, entityZ_field);

        //Sets entity's position to its current position, forcing MC to update its BB (for stationary entities)
        Minecraft::env->CallObjectMethod(entity, SetPosition, entityX, entityY, entityZ);

        double entitybb_x = (x1 + x2) / 2.0;
        double entitybb_y = (y1 + y2) / 2.0;
        double entitybb_z = (z1 + z2) / 2.0;

        // Direction player is looking
        jobject player_look_vec = Minecraft::env->CallObjectMethod(player, GetPlayerLook, 1.0F);

        //Postion of player eyes - used for player position
        jobject player_eye_pos = Minecraft::env->CallObjectMethod(player, GetPlayerEyes, 0.0F);

        // Player look direction extracted
        double player_lookX = Minecraft::env->GetDoubleField(player_look_vec, vecX);
        double player_lookY = Minecraft::env->GetDoubleField(player_look_vec, vecY);
        double player_lookZ = Minecraft::env->GetDoubleField(player_look_vec, vecZ);

        //Plaer eye position extracted
        double playerX = Minecraft::env->GetDoubleField(player_eye_pos, vecX);
        double playerY = Minecraft::env->GetDoubleField(player_eye_pos, vecY);
        double playerZ = Minecraft::env->GetDoubleField(player_eye_pos, vecZ);

        //This should be calculated with the entity's real position, not its BB's position

        double deltaX = playerX - entityX;
        double deltaY = playerY - (entityY + 1.5);
        double deltaZ = playerZ - entityZ;

        // Normalize the direction vector to length 1 to be able to scale it consistently
        double p2e_distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

        if (p2e_distance > 5.0 || p2e_distance < 3.0)
            continue;

        deltaX /= p2e_distance;
        deltaY /= p2e_distance;
        deltaZ /= p2e_distance;

        double entity_deltaX = entityX - entitybb_x;
        double entity_deltaY = entityY - entitybb_y;
        double entity_deltaZ = entityZ - entitybb_z;

        double e2bb_distance = sqrt(entity_deltaX * entity_deltaX + entity_deltaZ * entity_deltaZ);

        double dot = deltaX * player_lookX + deltaY * player_lookY + deltaZ * player_lookZ;

        //If dot is smaller than 0.6, the entity is outside of a 53 degree cone in front
        //of the player, and we do not want to shift its BB.
        if (dot > -0.96)
            continue;

        //Scale = how far to move the hitbox towards us in blocks
        double scale = 0.3;
        deltaX *= scale;
        deltaY *= scale;
        deltaZ *= scale;

        if (e2bb_distance > 0.2)
            continue;

        jobject closer_bb = Minecraft::env->NewObject(aabb_class, CreateAABB,
            x1 + deltaX, y1 + deltaY, z1 + deltaZ,
            x2 + deltaX, y2 + deltaY, z2 + deltaZ);

        Minecraft::env->CallVoidMethod(entity, SetBB, closer_bb);
    }
}