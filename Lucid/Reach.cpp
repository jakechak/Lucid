#include "pch.h"
#include "Reach.h"
#include "Client.h"

Reach::Reach() : Module(Client::env->NewStringUTF("Reach"), 0x30)
{
    aabb_class = Client::env->FindClass("aug");

    GetLoadedEntities = Client::env->GetMethodID(Client::worldc_class, "E", "()Ljava/util/List;");
    GetEntityBB = Client::env->GetMethodID(Client::entity_class, "aR", "()Laug;");

    GetPlayerLook = Client::env->GetMethodID(Client::entity_class, "d", "(F)Laui;");
    GetPlayerEyes = Client::env->GetMethodID(Client::entity_class, "e", "(F)Laui;");

    SetPosition = Client::env->GetMethodID(Client::entity_class, "b", "(DDD)V");
    SetBB = Client::env->GetMethodID(Client::entity_class, "a", "(Laug;)V");

    CreateAABB = Client::env->GetMethodID(aabb_class, "<init>", "(DDDDDD)V");

    bb_minX = Client::env->GetFieldID(aabb_class, "a", "D");
    bb_minY = Client::env->GetFieldID(aabb_class, "b", "D");
    bb_minZ = Client::env->GetFieldID(aabb_class, "c", "D");
    bb_maxX = Client::env->GetFieldID(aabb_class, "d", "D");
    bb_maxY = Client::env->GetFieldID(aabb_class, "e", "D");
    bb_maxZ = Client::env->GetFieldID(aabb_class, "f", "D");

    entityX_field = Client::env->GetFieldID(Client::entity_class, "s", "D");
    entityY_field = Client::env->GetFieldID(Client::entity_class, "t", "D");
    entityZ_field = Client::env->GetFieldID(Client::entity_class, "u", "D");

    vecX = Client::env->GetFieldID(Client::vec_class, "a", "D");
    vecY = Client::env->GetFieldID(Client::vec_class, "b", "D");
    vecZ = Client::env->GetFieldID(Client::vec_class, "c", "D");
}

void Reach::OnUpdate()
{
	if (!this->IsToggled())
		return;

    jobject player = Client::env->GetObjectField(Client::mc_inst, Client::player_field);
    jobject worldc = Client::env->GetObjectField(Client::mc_inst, Client::worldc_field);
    jobject entity_list = Client::env->CallObjectMethod(worldc, GetLoadedEntities);

    jclass list_class = Client::env->GetObjectClass(entity_list);
    jmethodID GetListSize = Client::env->GetMethodID(list_class, "size", "()I");
    jmethodID GetEntityFromList = Client::env->GetMethodID(list_class, "get", "(I)Ljava/lang/Object;");

    jint list_size = Client::env->CallIntMethod(entity_list, GetListSize);

    for (jint i = 0; i < list_size; i++)
    {
        jobject entity = Client::env->CallObjectMethod(entity_list, GetEntityFromList, i);

        if (!Client::env->IsInstanceOf(entity, Client::livingentitybase_class) || Client::env->IsSameObject(entity, player))
            continue;

        jobject entity_bb = Client::env->CallObjectMethod(entity, GetEntityBB);

        double x1 = Client::env->GetDoubleField(entity_bb, bb_minX);
        double y1 = Client::env->GetDoubleField(entity_bb, bb_minY);
        double z1 = Client::env->GetDoubleField(entity_bb, bb_minZ);
        double x2 = Client::env->GetDoubleField(entity_bb, bb_maxX);
        double y2 = Client::env->GetDoubleField(entity_bb, bb_maxY);
        double z2 = Client::env->GetDoubleField(entity_bb, bb_maxZ);

        double entityX = Client::env->GetDoubleField(entity, entityX_field);
        double entityY = Client::env->GetDoubleField(entity, entityY_field);
        double entityZ = Client::env->GetDoubleField(entity, entityZ_field);

        //Sets entity's position to its current position, forcing MC to update its BB (for stationary entities)
        Client::env->CallObjectMethod(entity, SetPosition, entityX, entityY, entityZ);

        double entitybb_x = (x1 + x2) / 2.0;
        double entitybb_y = (y1 + y2) / 2.0;
        double entitybb_z = (z1 + z2) / 2.0;

        // Direction player is looking
        jobject player_look_vec = Client::env->CallObjectMethod(player, GetPlayerLook, 1.0F);

        //Postion of player eyes - used for player position
        jobject player_eye_pos = Client::env->CallObjectMethod(player, GetPlayerEyes, 0.0F);

        // Player look direction extracted
        double player_lookX = Client::env->GetDoubleField(player_look_vec, vecX);
        double player_lookY = Client::env->GetDoubleField(player_look_vec, vecY);
        double player_lookZ = Client::env->GetDoubleField(player_look_vec, vecZ);

        //Plaer eye position extracted
        double playerX = Client::env->GetDoubleField(player_eye_pos, vecX);
        double playerY = Client::env->GetDoubleField(player_eye_pos, vecY);
        double playerZ = Client::env->GetDoubleField(player_eye_pos, vecZ);

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

        jobject closer_bb = Client::env->NewObject(aabb_class, CreateAABB,
            x1 + deltaX, y1 + deltaY, z1 + deltaZ,
            x2 + deltaX, y2 + deltaY, z2 + deltaZ);

        Client::env->CallVoidMethod(entity, SetBB, closer_bb);
    }
}