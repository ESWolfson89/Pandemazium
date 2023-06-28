// Copyright Eric Wolfson 2016-2017
// See LICENSE.txt (GPLv3)

#include "entity.h"

entity::entity()
{
    entity_id = -1;
    texture_index = 0;
    texture_row = 0;
    loc = point(0,0);
    textureDim = dim = point(2,2);
    center = point(1,1);
}

entity::entity(point l, point d, int tid, int eid)
{
    setEntityFields(l,d,tid,eid);
}

void entity::setEntityFields(point l, point d, int tid, int eid)
{
    loc = l;
    dim = d;
    textureDim = dim;
    entity_id = eid;
    texture_index = tid;
    center = getMidpoint(loc,addPoints(loc,dim));
    max_loc = addPoints(loc,dim);
}

point entity::getLoc()
{
    return loc;
}

point entity::getDim()
{
    return dim;
}

point entity::getTextureDim()
{
    return textureDim;
}

point entity::getCenter()
{
    return center;
}

point entity::getMaxLoc()
{
    return max_loc;
}

int entity::getTextureIndex()
{
    return texture_index;
}

int entity::getTextureRow()
{
    return texture_row;
}

int entity::entid()
{
    return entity_id;
}

void entity::setDim(point new_dim)
{
    dim = new_dim;
}

void entity::setTextureDim(point new_dim)
{
    textureDim = new_dim;
}

void entity::setTextureRow(int row)
{
    texture_row = row;
}

void entity::setLoc(point l)
{
    loc = l;
    setCenter();
    setMaxLoc();
}

void entity::setCenter()
{
    center = getMidpoint(loc,addPoints(loc,dim));
}

void entity::setMaxLoc()
{
    max_loc = addPoints(loc,dim);
}

static_entity::static_entity()
{
    blocking = true;
    destructable = false;
    color_id = -1;
}

void static_entity::setDestructable(bool dstr)
{
    destructable = dstr;
}

void static_entity::setColorID(int cid) {
    color_id = cid;
}

bool static_entity::isBlocking()
{
    return blocking;
}

bool static_entity::isDestructable()
{
    return destructable;
}

int static_entity::getColorID()
{
    return color_id;
}

dynamic_entity::dynamic_entity()
{
    velocity = point(0,0);
    old_loc = loc;
    vertical_motion_flag = true;
    horizontal_motion_flag = false;
    x_orientation = SDL_FLIP_NONE;
    xdelta_normal = 0.0;
    horizontal_stop_velocity = 0.1;
    horizontal_velocity_decrement = 1.0;
    speed_up_factor = 1.0;
    num_frames = 1;
    current_frame = 0;
    animation_speed = 1;
    animation_speed_counter = 0;
    frame_increment = 1;
    is_animating = false;
    reset_frame_when_done = true;
    destroy_upon_collision = false;
    mark_for_deletion = false;
    phys_stat = PHYSTYPE_FULL;
    animation_sequence_index = 0;
}

physics_type dynamic_entity::getPhysicsStatus()
{
    return phys_stat;
}

point dynamic_entity::getVelocity()
{
    return velocity;
}

point dynamic_entity::getMaxVelocity()
{
    return max_velocity;
}

point dynamic_entity::getOldLoc()
{
    return old_loc;
}

bool dynamic_entity::getVerticalMotionFlag()
{
    return vertical_motion_flag;
}

double dynamic_entity::getXDeltaNormal()
{
    return xdelta_normal;
}

SDL_RendererFlip  dynamic_entity::getXOrientation()
{
    return x_orientation;
}

void dynamic_entity::limitVelocity()
{
	if (velocity.x() < -max_velocity.x())
		velocity.setx(-max_velocity.x());
	if (velocity.x() > max_velocity.x())
		velocity.setx(max_velocity.x());
	if (velocity.y() < -max_velocity.y())
		velocity.sety(-max_velocity.y());
	if (velocity.y() > max_velocity.y())
		velocity.sety(max_velocity.y());
}

void dynamic_entity::setVelocity(point new_vel)
{
    velocity = new_vel;
    limitVelocity();
}

void dynamic_entity::incVelocity(point summand)
{
    velocity = addPoints(velocity,summand);
    limitVelocity();
}

void dynamic_entity::setMaxVelocity(point mv)
{
    max_velocity = mv;
}

void dynamic_entity::setVerticalMotionFlag(bool f)
{
    vertical_motion_flag = f;
}

void dynamic_entity::setDynamicEntityFields(initial_dynamic_entity_fields idef, point pos, int id)
{
    setEntityFields(pos,idef.dimensions,idef.texture_index,id);
    max_velocity = idef.max_velocity;
    horizontal_stop_velocity = idef.horizontal_stop_velocity;
    horizontal_velocity_decrement = idef.horizontal_velocity_decrement;
    speed_up_factor = idef.speed_up_factor;
    num_frames = idef.num_frames;
    animation_speed = idef.animation_speed;
}

void dynamic_entity::setXDeltaNormal(double xdn)
{
    xdelta_normal = xdn;
}

void dynamic_entity::restrictLoc(point minp, point maxp)
{
    if (old_loc.y() >= maxp.y()) {
        loc.sety(maxp.y()+1.0);
        velocity.set(velocity.x(),2.6);
    }
    else if (old_loc.y() <= minp.y() - dim.y()) {
        setVerticalMotionFlag(false);
        loc.sety(minp.y() - dim.y());
        velocity.sety(0.0);
    }
    else if (old_loc.x() >= maxp.x()) {
        loc.setx(maxp.x()+1.0);
        velocity.setx(0.5);
    }
    else if (old_loc.x() <= minp.x() - dim.x()) {
        loc.setx(minp.x() - dim.x()-1.0);
        velocity.setx(-0.5);
    }

    setCenter();
    setMaxLoc();
}

void dynamic_entity::setMarkForDeletion()
{
    mark_for_deletion = true;
}

void dynamic_entity::incLoc(point l)
{
    loc = addPoints(loc,l);
    setCenter();
    setMaxLoc();
}

void dynamic_entity::saveLoc()
{
    old_loc = loc;
}

void dynamic_entity::setXOrientation(SDL_RendererFlip srf)
{
    x_orientation = srf;
}

void dynamic_entity::setCurrentFrame(int f)
{
    current_frame = f;
}

void dynamic_entity::setFrameIncrement(int i)
{
    frame_increment = i;
}

void dynamic_entity::setAnimationSpeed(int spd)
{
    animation_speed = spd;
}

void dynamic_entity::animate(int lower_bound, int upper_bound)
{
    if (is_animating)
    {
        animation_speed_counter++;
        if (animation_speed_counter % animation_speed == 0)
            current_frame += frame_increment;
        if (current_frame > upper_bound || current_frame < lower_bound)
        {
            current_frame -= frame_increment;
            setAnimationStatus(false);
        }
    }
}

void dynamic_entity::animateCustomSequence(AnimationSequence animation_sequence)
{
    if (is_animating)
    {
        animation_speed_counter++;

        if (animation_speed_counter % animation_speed == 0) {
            animation_sequence_index++;
            if (animation_sequence_index > animation_sequence.sequence.size() - 1)
            {
                animation_sequence_index = 0;
            }
            current_frame = animation_sequence.sequence[animation_sequence_index];
        }
    }
}

void dynamic_entity::setAnimationStatus(bool f)
{
    is_animating = f;
    if (!is_animating)
    {
        animation_speed_counter = 0;
        if (reset_frame_when_done)
            current_frame = 0;
    }
}

void dynamic_entity::setResetFrameFlag(bool f)
{
    reset_frame_when_done = f;
}

void dynamic_entity::togglePhysics(physics_type pst)
{
    phys_stat = pst;
}

bool dynamic_entity::destroyUponCollision()
{
    return destroy_upon_collision;
}

int dynamic_entity::getCurrentFrame()
{
    return current_frame;
}

int dynamic_entity::getAnimationSpeed()
{
    return animation_speed;
}

int dynamic_entity::getFrameIncrement()
{
    return frame_increment;
}

bool dynamic_entity::isAnimating()
{
    return is_animating;
}

bool dynamic_entity::getResetFrameFlag()
{
    return reset_frame_when_done;
}

bool dynamic_entity::getMarkForDeletion()
{
    return mark_for_deletion;
}

double dynamic_entity::horizontalStopVelocity()
{
    return horizontal_stop_velocity;
}

double dynamic_entity::horizontalVelocityDecrement()
{
    return horizontal_velocity_decrement;
}

double dynamic_entity::speedUpFactor()
{
    return speed_up_factor;
}

void dynamic_entity::setDestructableUponCollision(bool flag) {
    destroy_upon_collision = flag;
}

int dynamic_entity::getNumFrames()
{
    return num_frames;
}

item::item()
{
    possession_mob_id = -1;
    usage_timer = 0;
    hold_timer = 0;
    can_use = true;
    wm_type = WEAPONMODIFIER_NONE;
}

int item::getPossessionMobID()
{
    return possession_mob_id;
}

int item::getHoldTimer()
{
    return hold_timer;
}

bool item::getUsability()
{
    return can_use;
}

void item::setUsability(bool u)
{
    can_use = u;
}

void item::setItemFields(initial_item_fields iif, point pos, int id)
{
    setDynamicEntityFields(iif.idef,pos,id);
    ii_sfields = iif.iisf;
    reset_frame_when_done = true;
    setXDeltaNormal(0.0);
    wm_type = WEAPONMODIFIER_NONE;
}

void item::setWeaponModifierType(weaponmodifier_type wmt)
{
    wm_type = wmt;

    if (wm_type == WEAPONMODIFIER_FAST || wm_type == WEAPONMODIFIER_FASTDAMAGING)
    {
        ii_sfields.usability_speed /= 2;

        if (ii_sfields.usability_speed < 1)
            ii_sfields.usability_speed = 1;

        if (animation_speed > 1)
            animation_speed /= 2;
    }

    if (wm_type == WEAPONMODIFIER_DAMAGING || wm_type == WEAPONMODIFIER_FASTDAMAGING)
    {
        ii_sfields.wep_stats.hp_decrement *= 2;

        if (ii_sfields.wep_stats.knock_back > 0.0)
            ii_sfields.wep_stats.knock_back *= 2.0;
    }
}

void item::incUsageTimer()
{
    if (!can_use)
    {
        usage_timer++;
        if (usage_timer % ii_sfields.usability_speed == 0)
            usage_timer = 0;
        if (usage_timer == 0)
            can_use = true;
    }
}

void item::incHoldTimer()
{
    hold_timer++;
    if (hold_timer >= 30)
        hold_timer = 30;
}

void item::resetHoldTimer()
{
    hold_timer = 0;
}

void item::setPossessionMobID(int mid)
{
    possession_mob_id = mid;
}

item_type item::getItemType()
{
    return ii_sfields.i_type;
}

item_category item::getItemCategory()
{
    return ii_sfields.i_category;
}

weaponmodifier_type item::getWeaponModifierType()
{
    return wm_type;
}

initial_item_super_fields * item::getItemSuperFields()
{
    return &ii_sfields;
}

mob::mob()
{
    moving_fast = true;
    item_carry_id = -1;
    is_dead = false;
    aggroed = false;
    on_ladder = false;
    move_status = MOVETYPE_STANDINGSTILL;
    item_carry_type = ITEMTYPE_NONE;
    mm_type = MOBMODIFIER_NONE;
    hitpoints = 10;
    dangerLevel = 1;
    tilt = 0.0;
    name = "";
    mob_death_type = MOBDEATHTYPE_NONE;
    burningCounter = 0;
}

mobmodifier_type mob::getMobModifierType()
{
    return mm_type;
}

item_type mob::getItemCarryType()
{
    return item_carry_type;
}

bool mob::movingFast()
{
    return moving_fast;
}

void mob::setMoveFastFlag(bool f)
{
    moving_fast = f;
}

void mob::setItemCarryID(int c_id)
{
    item_carry_id = c_id;
}

void mob::setDeathStatus(bool d)
{
    is_dead = d;
}

void mob::setMobDeathType(MobDeathType mob_death_type) {
    this->mob_death_type = mob_death_type;
}

void mob::setMobModifierType(mobmodifier_type mmt)
{
    mm_type = mmt;

    if (mm_type == MOBMODIFIER_FASTTOUGH || mm_type == MOBMODIFIER_TOUGH)
    {
        im_sfields.base_exp_worth *= 5;
        im_sfields.max_hp *= 2;
        hitpoints = im_sfields.max_hp;
    }

    if (mm_type == MOBMODIFIER_FASTTOUGH || mm_type == MOBMODIFIER_FAST)
    {
        im_sfields.base_exp_worth *= 5;
        max_velocity.setx(max_velocity.x()*2.0);
    }
}

void mob::setAggroStatus(bool agg)
{
    aggroed = agg;
}

void mob::setMobFields(initial_mob_fields imf, point pos, int id, double x_delta_start)
{
    setDynamicEntityFields(imf.idef,pos,id);
    im_sfields = imf.imsf;
    setHP(imf.imsf.max_hp);
    reset_frame_when_done = false;
    setXDeltaNormal(x_delta_start);
    mm_type = MOBMODIFIER_NONE;
}

void mob::setHP(int hp)
{
    hitpoints = hp;
}

void mob::incHP(int hp)
{
    hitpoints += hp;
    if (hitpoints > im_sfields.max_hp)
        hitpoints = im_sfields.max_hp;
}

void mob::setDangerLevel(int dl)
{
    double sizeScaling;
    dangerLevel = dl;
    if (getMobType() == MOB_SHADOW)
        sizeScaling = 1.0 + 0.4*(dl - 1);
    else
        sizeScaling = 1.0 + 0.2*(dl - 1);

    if (dangerLevel > 1) {
        if (getMobType() != MOB_EXECUTIONER)
            dim = multPoints(point(sizeScaling,sizeScaling), dim);
        else
            dim = multPoints(point(1,sizeScaling), dim);
        dim = point(floor(dim.x()),floor(dim.y()));
        setLoc(loc);
        setMaxHP(im_sfields.max_hp + 50.0*(dl - 1));
        setHP(im_sfields.max_hp);
        im_sfields.base_exp_worth += ((im_sfields.base_exp_worth/3) * (dl-1));
        im_sfields.melee_damage += 10 * (dl - 1);
    }
}

void mob::setItemCarryType(item_type ict)
{
    item_carry_type = ict;
}

void mob::setMoveStatus(move_type mt)
{
    move_status = mt;
}

void mob::setLadderStatus(bool ls)
{
    on_ladder = ls;
}

void mob::setBurningCounter(int burning_counter) {
    burningCounter = burning_counter;
}

bool mob::isDead()
{
    return is_dead;
}

bool mob::isAggroed()
{
    return aggroed;
}

int mob::getBurningCounter() {
    return burningCounter;
}

int mob::getItemCarryID()
{
    return item_carry_id;
}

int mob::getDangerLevel()
{
    return dangerLevel;
}

int mob::getHP()
{
    return hitpoints;
}

mob_type mob::getMobType()
{
    return im_sfields.m_type;
}

move_type mob::getMoveStatus()
{
    return move_status;
}

MobDeathType mob::getMobDeathType() {
    return mob_death_type;
}

initial_mob_super_fields * mob::getMobSuperFields()
{
    return &im_sfields;
}

void mob::incTilt()
{
    tilt+=1.0;
    if (tilt > 8.0)
        tilt = 0.0;
}

void mob::setTilt(double t)
{
    tilt = t;
}

double mob::getTilt()
{
    return tilt;
}

bool mob::getLadderStatus()
{
    return on_ladder;
}

bool mob::dropsKey()
{
    return drops_key;
}

void mob::setMaxHP(int nmhp)
{
    im_sfields.max_hp = nmhp;
}

void mob::setKeyDropFlag(bool kdf)
{
    drops_key = kdf;
}

void mob::setName(std::string n)
{
    name = n;
}

std::string mob::getName()
{
    return name;
}

particle::particle()
{
    creator_id = -1;
    damage_value = 0;
    knock_back = 0.0;
}

int particle::getCreatorID()
{
    return creator_id;
}

int particle::getDamageValue()
{
    return damage_value;
}

double particle::getKnockBack()
{
    return knock_back;
}

void particle::setParticleFields(initial_dynamic_entity_fields idef, point pos, int id, int cid, int dam_val, particle_type pt, double kb, item_type wep_assoc)
{
    setDynamicEntityFields(idef,pos,id);
    creator_id = cid;
    damage_value = dam_val;
    p_type = pt;
    knock_back = kb;
    weapon_associated = wep_assoc;
}

item_type particle::getWeaponAssociated()
{
    return weapon_associated;
}

particle_type particle::getParticleType()
{
    return p_type;
}

door::door()
{
    current_state = DOORSTATE_CLOSED;
    use_speed = 1.0;
    opened_loc = point(0,0);
    closed_loc = point(0,0);
    locked = false;
}

door_state door::getDoorState()
{
    return current_state;
}

double door::useSpeed()
{
    return use_speed;
}

void door::changeLoc(point l)
{
    loc = l;
    center = getMidpoint(loc,addPoints(loc,dim));
    max_loc = addPoints(loc,dim);
}

void door::setDoorState(door_state ds)
{
    current_state = ds;
}

void door::setDoorFields(point l, point sze, int tid, int eid, door_state ds, double uspd, bool lck, door_sizetype dst)
{
    setEntityFields(l,sze,tid,eid);

    current_state = ds;
    use_speed = uspd;
    opened_loc = addPoints(loc,point(0.0,-1.0*sze.y()));
    closed_loc = loc;

    if (current_state == DOORSTATE_CLOSED)
        changeLoc(closed_loc);
    if (current_state == DOORSTATE_OPENED)
        changeLoc(opened_loc);

    locked = lck;

    d_sizetype = dst;
}

void door::activateDoor()
{
    if (current_state == DOORSTATE_OPENING)
        changeLoc(point(loc.x(),loc.y() - use_speed));
    if (current_state == DOORSTATE_CLOSING)
        changeLoc(point(loc.x(),loc.y() + use_speed));

    if (loc.y() >= closed_loc.y())
    {
        changeLoc(closed_loc);
        current_state = DOORSTATE_CLOSED;
    }
    if (loc.y() <= opened_loc.y())
    {
        changeLoc(opened_loc);
        current_state = DOORSTATE_OPENED;
    }
}

void door::setLockStatus(bool ls)
{
    locked = ls;

    if (locked)
        texture_index = 29;
    else
        texture_index = 24;
}

bool door::isLocked()
{
    return locked;
}

door_sizetype door::getSizeType()
{
    return d_sizetype;
}

toggle_switch::toggle_switch()
{
    level_feature_id = 0;
}

int toggle_switch::getLevelFeatureID()
{
    return level_feature_id;
}

void toggle_switch::setSwitchFields(point l,point sze,int tid,int eid,int lfid)
{
    setEntityFields(l,sze,tid,eid);
    level_feature_id = lfid;
}

bool isDoorStationary(door_state ds) {
     return (ds == DOORSTATE_OPENED || ds == DOORSTATE_CLOSED);
}

bool collisionWithEntity(point center, point dim, entity *ent)
{
    return inRange(center, point(ent->getLoc().x() - (dim.x() / 2.0),
                                 ent->getLoc().y() - (dim.y() / 2.0)),
                           point(ent->getMaxLoc().x() + (dim.x() / 2.0),
                                 ent->getMaxLoc().y() + (dim.y() / 2.0)));
}

void checkMarkForDeletion(dynamic_entity *de) {
     if (de->destroyUponCollision())
         de->setMarkForDeletion();
}

void processDynamicEntityWallCollision(dynamic_entity *de, static_entity *wl) {
     if (collisionWithEntity(de->getCenter(), de->getDim(), wl)) {
         checkMarkForDeletion(de);
         de->restrictLoc(wl->getLoc(), wl->getMaxLoc());
     }
}

void processDynamicEntityDoorCollision(dynamic_entity *de, door *dr) {
     if (collisionWithEntity(de->getCenter(), de->getDim(), dr)) {
         checkMarkForDeletion(de);

         if (isDoorStationary(dr->getDoorState()))
             de->restrictLoc(dr->getLoc(), dr->getMaxLoc());
         else
             dr->setDoorState(DOORSTATE_OPENING);
     }
}

wall::wall() {
     tint = color_black;
}

SDL_Color wall::getTint() {
     return tint;
}

wall_type wall::getWallType() {
     return wl_type;
}

void wall::setWallFeatures(point l,point sze,int tid,int eid, SDL_Color t, wall_type wt) {
   setEntityFields(l,sze,tid,eid);
   tint = t;
   wl_type = wt;
}

Ladder::Ladder() {
    ladderTint = color_black;
}

SDL_Color Ladder::getLadderTint() {
    return ladderTint;
}

LadderType Ladder::getLadderType() {
    return ladderType;
}

LadderSnap Ladder::getLadderSnap() {
    return ladderSnap;
}

void Ladder::setLadderFeatures(point l, point size, point blockSize, int eID, SDL_Color lTint, LadderType lType, LadderSnap lSnap) {
    ladderTint = lTint;
    ladderType = lType;
    ladderSnap = lSnap;

    setEntityFields(l,size,getLadderTexture(ladderType),eID);

    switch(ladderSnap) {
        case(LADDERSNAP_CENTER):
            loc.setx(loc.x() + (blockSize.x()/2.0) - (size.x()/2.0));
            break;
        case(LADDERSNAP_RIGHT):
            loc.setx(loc.x() + blockSize.x() - size.x());
            break;
        case(LADDERSNAP_LEFT):
            loc.setx(loc.x() + 4.0);
            break;
        default:
            break;
    }

    center = getMidpoint(loc,addPoints(loc, dim));
    max_loc = addPoints(loc,dim);
}

int getLadderTexture(LadderType ladderType) {
    switch (ladderType) {
        case(LADDERTYPE_POLE):
            return 60;
        default:
            break;
    }

    return 16;
}

