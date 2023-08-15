#ifndef ENTITY_H_
#define ENTITY_H_

enum entity_flags {
    FL_ONGROUND              = (1 << 0),
    FL_DUCKING               = (1 << 1),
    FL_WATERJUMP             = (1 << 2),
    FL_ONTRAIN               = (1 << 3),
    FL_INRAIN                = (1 << 4),
    FL_FROZEN                = (1 << 5),
    FL_ATCONTROLS            = (1 << 6),
    FL_CLIENT                = (1 << 7),
    FL_FAKECLIENT            = (1 << 8),
    FL_INWATER               = (1 << 9),
    FL_FLY                   = (1 << 10),
    FL_SWIM                  = (1 << 11),
    FL_CONVEYOR              = (1 << 12),
    FL_NPC                   = (1 << 13),
    FL_GODMODE               = (1 << 14),
    FL_NOTARGET              = (1 << 15),
    FL_AIMTARGET             = (1 << 16),
    FL_PARTIALGROUND         = (1 << 17),
    FL_STATICPROP            = (1 << 18),
    FL_GRAPHED               = (1 << 19),
    FL_GRENADE               = (1 << 20),
    FL_STEPMOVEMENT          = (1 << 21),
    FL_DONTTOUCH             = (1 << 22),
    FL_BASEVELOCITY          = (1 << 23),
    FL_WORLDBRUSH            = (1 << 24),
    FL_OBJECT                = (1 << 25),
    FL_KILLME                = (1 << 26),
    FL_ONFIRE                = (1 << 27),
    FL_DISSOLVING            = (1 << 28),
    FL_TRANSRAGDOLL          = (1 << 29),
    FL_UNBLOCKABLE_BY_PLAYER = (1 << 30)
};

enum ETFClientClass {
    CClass_CTFWearableRazorback                  = 341,
    CClass_CTFWearableDemoShield                 = 338,
    CClass_CTFWearableLevelableItem              = 340,
    CClass_CTFWearableCampaignItem               = 337,
    CClass_CTFBaseRocket                         = 185,
    CClass_CTFWeaponBaseMerasmusGrenade          = 325,
    CClass_CTFWeaponBaseMelee                    = 324,
    CClass_CTFWeaponBaseGun                      = 323,
    CClass_CTFWeaponBaseGrenadeProj              = 322,
    CClass_CTFWeaponBase                         = 321,
    CClass_CTFWearableRobotArm                   = 342,
    CClass_CTFRobotArm                           = 287,
    CClass_CTFWrench                             = 344,
    CClass_CTFProjectile_ThrowableBreadMonster   = 279,
    CClass_CTFProjectile_ThrowableBrick          = 280,
    CClass_CTFProjectile_ThrowableRepel          = 281,
    CClass_CTFProjectile_Throwable               = 278,
    CClass_CTFThrowable                          = 319,
    CClass_CTFSyringeGun                         = 315,
    CClass_CTFKatana                             = 225,
    CClass_CTFSword                              = 314,
    CClass_CSniperDot                            = 118,
    CClass_CTFSniperRifleClassic                 = 308,
    CClass_CTFSniperRifleDecap                   = 309,
    CClass_CTFSniperRifle                        = 307,
    CClass_CTFChargedSMG                         = 197,
    CClass_CTFSMG                                = 306,
    CClass_CTFSlap                               = 305,
    CClass_CTFShovel                             = 304,
    CClass_CTFShotgunBuildingRescue              = 303,
    CClass_CTFPEPBrawlerBlaster                  = 241,
    CClass_CTFSodaPopper                         = 310,
    CClass_CTFShotgun_Revenge                    = 301,
    CClass_CTFScatterGun                         = 297,
    CClass_CTFShotgun_Pyro                       = 300,
    CClass_CTFShotgun_HWG                        = 299,
    CClass_CTFShotgun_Soldier                    = 302,
    CClass_CTFShotgun                            = 298,
    CClass_CTFRocketPack                         = 296,
    CClass_CTFCrossbow                           = 201,
    CClass_CTFRocketLauncher_Mortar              = 295,
    CClass_CTFRocketLauncher_AirStrike           = 293,
    CClass_CTFRocketLauncher_DirectHit           = 294,
    CClass_CTFRocketLauncher                     = 292,
    CClass_CTFRevolver                           = 286,
    CClass_CTFDRGPomson                          = 202,
    CClass_CTFRaygun                             = 284,
    CClass_CTFPistol_ScoutSecondary              = 246,
    CClass_CTFPistol_ScoutPrimary                = 245,
    CClass_CTFPistol_Scout                       = 244,
    CClass_CTFPistol                             = 243,
    CClass_CTFPipebombLauncher                   = 242,
    CClass_CTFWeaponPDA_Spy                      = 332,
    CClass_CTFWeaponPDA_Engineer_Destroy         = 331,
    CClass_CTFWeaponPDA_Engineer_Build           = 330,
    CClass_CTFWeaponPDAExpansion_Teleporter      = 334,
    CClass_CTFWeaponPDAExpansion_Dispenser       = 333,
    CClass_CTFWeaponPDA                          = 329,
    CClass_CTFParticleCannon                     = 239,
    CClass_CTFParachute_Secondary                = 238,
    CClass_CTFParachute_Primary                  = 237,
    CClass_CTFParachute                          = 236,
    CClass_CTFMinigun                            = 234,
    CClass_CTFMedigunShield                      = 231,
    CClass_CWeaponMedigun                        = 352,
    CClass_CTFProjectile_MechanicalArmOrb        = 263,
    CClass_CTFMechanicalArm                      = 230,
    CClass_CTFLunchBox_Drink                     = 229,
    CClass_CTFLunchBox                           = 228,
    CClass_CLaserDot                             = 78,
    CClass_CTFLaserPointer                       = 227,
    CClass_CTFKnife                              = 226,
    CClass_CTFGasManager                         = 212,
    CClass_CTFProjectile_JarGas                  = 261,
    CClass_CTFJarGas                             = 223,
    CClass_CTFProjectile_Cleaver                 = 254,
    CClass_CTFProjectile_JarMilk                 = 262,
    CClass_CTFProjectile_Jar                     = 260,
    CClass_CTFCleaver                            = 198,
    CClass_CTFJarMilk                            = 224,
    CClass_CTFJar                                = 222,
    CClass_CTFWeaponInvis                        = 328,
    CClass_CTFCannon                             = 196,
    CClass_CTFGrenadeLauncher                    = 216,
    CClass_CTFGrenadePipebombProjectile          = 217,
    CClass_CTFGrapplingHook                      = 215,
    CClass_CTFFlareGun_Revenge                   = 210,
    CClass_CTFFlareGun                           = 209,
    CClass_CTFFlameRocket                        = 207,
    CClass_CTFFlameThrower                       = 208,
    CClass_CTFFists                              = 205,
    CClass_CTFFireAxe                            = 204,
    CClass_CTFWeaponFlameBall                    = 327,
    CClass_CTFCompoundBow                        = 200,
    CClass_CTFClub                               = 199,
    CClass_CTFBuffItem                           = 195,
    CClass_CTFStickBomb                          = 312,
    CClass_CTFBreakableSign                      = 194,
    CClass_CTFBottle                             = 192,
    CClass_CTFBreakableMelee                     = 193,
    CClass_CTFBonesaw                            = 190,
    CClass_CTFBall_Ornament                      = 182,
    CClass_CTFStunBall                           = 313,
    CClass_CTFBat_Giftwrap                       = 188,
    CClass_CTFBat_Wood                           = 189,
    CClass_CTFBat_Fish                           = 187,
    CClass_CTFBat                                = 186,
    CClass_CTFProjectile_EnergyRing              = 256,
    CClass_CTFDroppedWeapon                      = 203, /* !!! */
    CClass_CTFWeaponSapper                       = 335,
    CClass_CTFWeaponBuilder                      = 326,
    CClass_C_TFWeaponBuilder                     = 0,
    CClass_CTFProjectile_Rocket                  = 264,
    CClass_CTFProjectile_Flare                   = 257,
    CClass_CTFProjectile_EnergyBall              = 255,
    CClass_CTFProjectile_GrapplingHook           = 258,
    CClass_CTFProjectile_HealingBolt             = 259,
    CClass_CTFProjectile_Arrow                   = 252,
    CClass_CMannVsMachineStats                   = 80,
    CClass_CTFTankBoss                           = 316,
    CClass_CTFBaseBoss                           = 183,
    CClass_CBossAlpha                            = 0,
    CClass_NextBotCombatCharacter                = 357,
    CClass_CTFProjectile_SpellKartBats           = 268,
    CClass_CTFProjectile_SpellKartOrb            = 269,
    CClass_CTFHellZap                            = 220,
    CClass_CTFProjectile_SpellLightningOrb       = 270,
    CClass_CTFProjectile_SpellTransposeTeleport  = 277,
    CClass_CTFProjectile_SpellMeteorShower       = 271,
    CClass_CTFProjectile_SpellSpawnBoss          = 274,
    CClass_CTFProjectile_SpellMirv               = 272,
    CClass_CTFProjectile_SpellPumpkin            = 273,
    CClass_CTFProjectile_SpellSpawnHorde         = 275,
    CClass_CTFProjectile_SpellSpawnZombie        = 276,
    CClass_CTFProjectile_SpellBats               = 266,
    CClass_CTFProjectile_SpellFireball           = 267,
    CClass_CTFSpellBook                          = 311,
    CClass_CHightower_TeleportVortex             = 74,
    CClass_CTeleportVortex                       = 160,
    CClass_CZombie                               = 354,
    CClass_CMerasmusDancer                       = 83,
    CClass_CMerasmus                             = 82,
    CClass_CHeadlessHatman                       = 73,
    CClass_CEyeballBoss                          = 48,
    CClass_CTFBotHintEngineerNest                = 191,
    CClass_CBotNPCMinion                         = 0,
    CClass_CBotNPC                               = 0,
    CClass_CPasstimeGun                          = 94,
    CClass_CTFViewModel                          = 320,
    CClass_CRobotDispenser                       = 112,
    CClass_CTFRobotDestruction_Robot             = 288,
    CClass_CTFReviveMarker                       = 285,
    CClass_CTFPumpkinBomb                        = 282,
    CClass_CTFProjectile_BallOfFire              = 253,
    CClass_CTFBaseProjectile                     = 184,
    CClass_CTFPointManager                       = 250,
    CClass_CBaseObjectUpgrade                    = 11,
    CClass_CTFRobotDestructionLogic              = 291,
    CClass_CTFRobotDestruction_RobotGroup        = 289,
    CClass_CTFRobotDestruction_RobotSpawn        = 290,
    CClass_CTFPlayerDestructionLogic             = 248,
    CClass_CPlayerDestructionDispenser           = 101,
    CClass_CTFMinigameLogic                      = 233,
    CClass_CTFHalloweenMinigame_FallingPlatforms = 219,
    CClass_CTFHalloweenMinigame                  = 218,
    CClass_CTFMiniGame                           = 232,
    CClass_CTFPowerupBottle                      = 251,
    CClass_CTFItem                               = 221,
    CClass_CHalloweenSoulPack                    = 71,
    CClass_CTFGenericBomb                        = 213,
    CClass_CBonusRoundLogic                      = 23,
    CClass_CTFGameRulesProxy                     = 211,
    CClass_CTETFParticleEffect                   = 179,
    CClass_CTETFExplosion                        = 178,
    CClass_CTETFBlood                            = 177,
    CClass_CTFFlameManager                       = 206,
    CClass_CHalloweenGiftPickup                  = 69,
    CClass_CBonusDuckPickup                      = 21,
    CClass_CHalloweenPickup                      = 70,
    CClass_CCaptureFlagReturnIcon                = 27,
    CClass_CCaptureFlag                          = 26,
    CClass_CBonusPack                            = 22,
    CClass_CTFTeam                               = 318,
    CClass_CTFTauntProp                          = 317,
    CClass_CTFPlayerResource                     = 249,
    CClass_CTFPlayer                             = 247, /* !!! */
    CClass_CTFRagdoll                            = 283,
    CClass_CTEPlayerAnimEvent                    = 165,
    CClass_CTFPasstimeLogic                      = 240,
    CClass_CPasstimeBall                         = 93,
    CClass_CTFObjectiveResource                  = 235,
    CClass_CTFGlow                               = 214,
    CClass_CTEFireBullets                        = 152,
    CClass_CTFBuffBanner                         = 0,
    CClass_CTFAmmoPack                           = 181, /* !!! */
    CClass_CObjectTeleporter                     = 89,  /* !!! */
    CClass_CObjectSentrygun                      = 88,  /* !!! */
    CClass_CTFProjectile_SentryRocket            = 265,
    CClass_CObjectSapper                         = 87,
    CClass_CObjectCartDispenser                  = 85,
    CClass_CObjectDispenser                      = 86, /* !!! */
    CClass_CMonsterResource                      = 84,
    CClass_CFuncRespawnRoomVisualizer            = 64,
    CClass_CFuncRespawnRoom                      = 63,
    CClass_CFuncPasstimeGoal                     = 61,
    CClass_CFuncForceField                       = 57,
    CClass_CCaptureZone                          = 28,
    CClass_CCurrencyPack                         = 31, /* !!! */
    CClass_CBaseObject                           = 10,
    CClass_CTestTraceline                        = 176,
    CClass_CTEWorldDecal                         = 180,
    CClass_CTESpriteSpray                        = 174,
    CClass_CTESprite                             = 173,
    CClass_CTESparks                             = 172,
    CClass_CTESmoke                              = 171,
    CClass_CTEShowLine                           = 169,
    CClass_CTEProjectedDecal                     = 167,
    CClass_CTEPlayerDecal                        = 166,
    CClass_CTEPhysicsProp                        = 164,
    CClass_CTEParticleSystem                     = 163,
    CClass_CTEMuzzleFlash                        = 162,
    CClass_CTELargeFunnel                        = 159,
    CClass_CTEKillPlayerAttachments              = 158,
    CClass_CTEImpact                             = 157,
    CClass_CTEGlowSprite                         = 156,
    CClass_CTEShatterSurface                     = 168,
    CClass_CTEFootprintDecal                     = 154,
    CClass_CTEFizz                               = 153,
    CClass_CTEExplosion                          = 151,
    CClass_CTEEnergySplash                       = 150,
    CClass_CTEEffectDispatch                     = 149,
    CClass_CTEDynamicLight                       = 148,
    CClass_CTEDecal                              = 146,
    CClass_CTEClientProjectile                   = 145,
    CClass_CTEBubbleTrail                        = 144,
    CClass_CTEBubbles                            = 143,
    CClass_CTEBSPDecal                           = 142,
    CClass_CTEBreakModel                         = 141,
    CClass_CTEBloodStream                        = 140,
    CClass_CTEBloodSprite                        = 139,
    CClass_CTEBeamSpline                         = 138,
    CClass_CTEBeamRingPoint                      = 137,
    CClass_CTEBeamRing                           = 136,
    CClass_CTEBeamPoints                         = 135,
    CClass_CTEBeamLaser                          = 134,
    CClass_CTEBeamFollow                         = 133,
    CClass_CTEBeamEnts                           = 132,
    CClass_CTEBeamEntPoint                       = 131,
    CClass_CTEBaseBeam                           = 130,
    CClass_CTEArmorRicochet                      = 129,
    CClass_CTEMetalSparks                        = 161,
    CClass_CSteamJet                             = 123,
    CClass_CSmokeStack                           = 117,
    CClass_DustTrail                             = 355,
    CClass_CFireTrail                            = 50,
    CClass_SporeTrail                            = 362,
    CClass_SporeExplosion                        = 361,
    CClass_RocketTrail                           = 359,
    CClass_SmokeTrail                            = 360,
    CClass_CPropVehicleDriveable                 = 108,
    CClass_ParticleSmokeGrenade                  = 358,
    CClass_CParticleFire                         = 90,
    CClass_MovieExplosion                        = 356,
    CClass_CTEGaussExplosion                     = 155,
    CClass_CEnvQuadraticBeam                     = 43,
    CClass_CEmbers                               = 36,
    CClass_CEnvWind                              = 47,
    CClass_CPrecipitation                        = 107,
    CClass_CBaseTempEntity                       = 17,
    CClass_CWeaponIFMSteadyCam                   = 351,
    CClass_CWeaponIFMBaseCamera                  = 350,
    CClass_CWeaponIFMBase                        = 349,
    CClass_CTFWearableVM                         = 343,
    CClass_CTFWearable                           = 336,
    CClass_CTFWearableItem                       = 339,
    CClass_CEconWearable                         = 35,
    CClass_CBaseAttributableItem                 = 3,
    CClass_CEconEntity                           = 34,
    CClass_CHandleTest                           = 72,
    CClass_CTeamplayRoundBasedRulesProxy         = 126,
    CClass_CTeamRoundTimer                       = 127,
    CClass_CSpriteTrail                          = 122,
    CClass_CSpriteOriented                       = 121,
    CClass_CSprite                               = 120,
    CClass_CRagdollPropAttached                  = 111,
    CClass_CRagdollProp                          = 110,
    CClass_CPoseController                       = 106,
    CClass_CGameRulesProxy                       = 68,
    CClass_CInfoLadderDismount                   = 75,
    CClass_CFuncLadder                           = 58,
    CClass_CEnvDetailController                  = 40,
    CClass_CWorld                                = 353,
    CClass_CWaterLODControl                      = 348,
    CClass_CWaterBullet                          = 347,
    CClass_CVoteController                       = 346,
    CClass_CVGuiScreen                           = 345,
    CClass_CPropJeep                             = 0,
    CClass_CPropVehicleChoreoGeneric             = 0,
    CClass_CTest_ProxyToggle_Networkable         = 175,
    CClass_CTesla                                = 170,
    CClass_CTeamTrainWatcher                     = 128,
    CClass_CBaseTeamObjectiveResource            = 16,
    CClass_CTeam                                 = 125,
    CClass_CSun                                  = 124,
    CClass_CParticlePerformanceMonitor           = 91,
    CClass_CSpotlightEnd                         = 119,
    CClass_CSlideshowDisplay                     = 116,
    CClass_CShadowControl                        = 115,
    CClass_CSceneEntity                          = 114,
    CClass_CRopeKeyframe                         = 113,
    CClass_CRagdollManager                       = 109,
    CClass_CPhysicsPropMultiplayer               = 98,
    CClass_CPhysBoxMultiplayer                   = 96,
    CClass_CBasePropDoor                         = 15,
    CClass_CDynamicProp                          = 33,
    CClass_CPointWorldText                       = 105,
    CClass_CPointCommentaryNode                  = 104,
    CClass_CPointCamera                          = 103,
    CClass_CPlayerResource                       = 102,
    CClass_CPlasma                               = 100,
    CClass_CPhysMagnet                           = 99,
    CClass_CPhysicsProp                          = 97,
    CClass_CPhysBox                              = 95,
    CClass_CParticleSystem                       = 92,
    CClass_CMaterialModifyControl                = 81,
    CClass_CLightGlow                            = 79,
    CClass_CInfoOverlayAccessor                  = 77,
    CClass_CFuncTrackTrain                       = 67,
    CClass_CFuncSmokeVolume                      = 66,
    CClass_CFuncRotating                         = 65,
    CClass_CFuncReflectiveGlass                  = 62,
    CClass_CFuncOccluder                         = 60,
    CClass_CFuncMonitor                          = 59,
    CClass_CFunc_LOD                             = 54,
    CClass_CTEDust                               = 147,
    CClass_CFunc_Dust                            = 53,
    CClass_CFuncConveyor                         = 56,
    CClass_CBreakableSurface                     = 25,
    CClass_CFuncAreaPortalWindow                 = 55,
    CClass_CFish                                 = 51,
    CClass_CEntityFlame                          = 38,
    CClass_CFireSmoke                            = 49,
    CClass_CEnvTonemapController                 = 46,
    CClass_CEnvScreenEffect                      = 44,
    CClass_CEnvScreenOverlay                     = 45,
    CClass_CEnvProjectedTexture                  = 42,
    CClass_CEnvParticleScript                    = 41,
    CClass_CFogController                        = 52,
    CClass_CEntityParticleTrail                  = 39,
    CClass_CEntityDissolve                       = 37,
    CClass_CDynamicLight                         = 32,
    CClass_CColorCorrectionVolume                = 30,
    CClass_CColorCorrection                      = 29,
    CClass_CBreakableProp                        = 24,
    CClass_CBasePlayer                           = 13,
    CClass_CBaseFlex                             = 8,
    CClass_CBaseEntity                           = 7,
    CClass_CBaseDoor                             = 6,
    CClass_CBaseCombatCharacter                  = 4,
    CClass_CBaseAnimatingOverlay                 = 2,
    CClass_CBoneFollower                         = 20,
    CClass_CBaseAnimating                        = 1, /* !!! */
    CClass_CInfoLightingRelative                 = 76,
    CClass_CAI_BaseNPC                           = 0,
    CClass_CBeam                                 = 19,
    CClass_CBaseViewModel                        = 18,
    CClass_CBaseProjectile                       = 14,
    CClass_CBaseParticleEntity                   = 12,
    CClass_CBaseGrenade                          = 9,
    CClass_CBaseCombatWeapon                     = 5
};

enum ETFClass {
    CLASS_NONE = 0,
    CLASS_SCOUT,
    CLASS_SNIPER,
    CLASS_SOLDIER,
    CLASS_DEMOMAN,
    CLASS_MEDIC,
    CLASS_HEAVY,
    CLASS_PYRO,
    CLASS_SPY,
    CLASS_ENGINEER
};

enum BoneMasks {
    BONE_USED_MASK           = 0x0007FF00,
    BONE_USED_BY_ANYTHING    = 0x0007FF00,
    BONE_USED_BY_HITBOX      = 0x00000100,
    BONE_USED_BY_ATTACHMENT  = 0x00000200,
    BONE_USED_BY_VERTEX_MASK = 0x0003FC00,
    BONE_USED_BY_VERTEX_LOD0 = 0x00000400,
    BONE_USED_BY_VERTEX_LOD1 = 0x00000800,
    BONE_USED_BY_VERTEX_LOD2 = 0x00001000,
    BONE_USED_BY_VERTEX_LOD3 = 0x00002000,
    BONE_USED_BY_VERTEX_LOD4 = 0x00004000,
    BONE_USED_BY_VERTEX_LOD5 = 0x00008000,
    BONE_USED_BY_VERTEX_LOD6 = 0x00010000,
    BONE_USED_BY_VERTEX_LOD7 = 0x00020000,
    BONE_USED_BY_BONE_MERGE  = 0x00040000,
};

enum ObsModes {
    OBS_MODE_NONE = 0,
    OBS_MODE_DEATHCAM,
    OBS_MODE_FREEZECAM,
    OBS_MODE_FIXED,
    OBS_MODE_IN_EYE,
    OBS_MODE_CHASE,
    OBS_MODE_POI,
    OBS_MODE_ROAMING,
    NUM_OBSERVER_MODES,
};

#define MAX_PLAYER_NAME_LENGTH 32
#define SIGNED_GUID_LEN        32
#define MAX_CUSTOM_FILES       4

typedef struct player_info_s {
    char name[MAX_PLAYER_NAME_LENGTH];
    int userID;
    char guid[SIGNED_GUID_LEN + 1];
    uint32_t friendsID;
    char friendsName[MAX_PLAYER_NAME_LENGTH];
    bool fakeplayer;
    bool ishltv;
    uint32_t customFiles[MAX_CUSTOM_FILES];
    unsigned char filesDownloaded;
} player_info_t;

/*----------------------------------------------------------------------------*/

typedef struct Collideable Collideable;
typedef struct Networkable Networkable;
typedef struct Renderable Renderable;
typedef struct Entity Entity;
typedef struct Weapon Weapon;

typedef struct {
    PAD(4 * 1);
    vec3_t* (*ObbMinsPreScaled)(Collideable*); /* 1 */
    vec3_t* (*ObbMaxsPreScaled)(Collideable*); /* 2 */
    vec3_t* (*ObbMins)(Collideable*);          /* 3 */
    vec3_t* (*ObbMaxs)(Collideable*);          /* 4 */
} VMT_Collideable;

struct Collideable {
    VMT_Collideable* vmt;
};

typedef struct {
    PAD(0x8);
    const char* network_name;
    PAD(0x8);
    int class_id; /* enum ETFClientClass */
} ClientClass;

typedef struct {
    PAD(4 * 2);
    ClientClass* (*GetClientClass)(Networkable*); /* 2 */
    PAD(4 * 5);
    bool (*IsDormant)(Networkable*); /* 8 */
    int (*GetIndex)(Networkable*);   /* 9 */
} VMT_Networkable;

struct Networkable {
    VMT_Networkable* vmt;
};

#define MAXSTUDIOBONES 128 /* Size of "bones" array */
typedef void model_t;
typedef struct {
    PAD(4 * 9);
    const model_t* (*GetModel)(Renderable*); /* 9 */
    PAD(4 * 6);
    bool (*SetupBones)(Renderable*, matrix3x4_t* bones, int maxBones,
                       int boneMask, float currentTime); /* 16 */
    PAD(4 * 17);
    matrix3x4_t* (*RenderableToWorldTransform)(Renderable*); /* 34 */
} VMT_Renderable;

struct Renderable {
    VMT_Renderable* vmt;
};

typedef struct {
    PAD(4 * 4);
    Collideable* (*GetCollideable)(Entity*); /* 4 */
    PAD(4 * 6);
    vec3_t* (*GetAbsOrigin)(Entity*); /* 11 */
    vec3_t* (*GetAbsAngles)(Entity*); /* 12 */
    PAD(4 * 66);
    int (*GetIndex)(Entity*); /* 79 */

    /* FIXME */
    PAD(4 * 31);
    vec3_t* (*WorldSpaceCenter)(Entity*); /* 111 */
    PAD(4 * 5);

    int (*GetTeamNumber)(Entity*); /* 117 */
    PAD(4 * 34);
    int (*GetHealth)(Entity*);    /* 152 */
    int (*GetMaxHealth)(Entity*); /* 153 */
    PAD(4 * 29);
    bool (*IsAlive)(Entity*);  /* 183 */
    bool (*IsPlayer)(Entity*); /* 184 */
    PAD(4 * 2);
    bool (*IsNPC)(Entity*); /* 187 */
    PAD(4 * 2);
    bool (*IsWeapon)(Entity*); /* 190 */
    PAD(4 * 3);
    vec3_t (*EyePosition)(Entity*); /* 194 */
    vec3_t (*EyeAngles)(Entity*);   /* 195 */
    PAD(4 * 95);
    Weapon* (*GetWeapon)(Entity*); /* 291 */
    PAD(4 * 17);
    int (*GetObserverMode)(Entity*);       /* 309 */
    Entity* (*GetObserverTarget)(Entity*); /* 310 */
} VMT_Entity;

struct Entity {
    VMT_Entity* vmt;
    PAD(0x7C);
    int model_idx; /* 0x80 */
    PAD(0x8C);
    vec3_t velocity; /* 0x110 */
    PAD(0x250);
    int flags; /* 0x36C */
    PAD(0x8E4);
    float flNextAttack; /* 0xC54 */
    PAD(0x214);
    vec3_t vecPunchAngle; /* 0xE6C */
    PAD(0x3BC);
    int nTickBase; /* 0x1234 */
    PAD(0x3F8);
    int player_class; /* 0x1630 (ETFClass) */
};

#define IsTeammate(ent) \
    (METHOD(g.localplayer, GetTeamNumber) == METHOD(ent, GetTeamNumber))

static inline Renderable* GetRenderable(Entity* ent) {
    return (Renderable*)((void*)ent + 0x4);
}

static inline Networkable* GetNetworkable(Entity* ent) {
    return (Networkable*)((void*)ent + 0x8);
}

static inline const char* GetClassName(Entity* ent) {
    switch (ent->player_class) {
        case CLASS_SCOUT:
            return "Scout";
        case CLASS_DEMOMAN:
            return "Demoman";
        case CLASS_ENGINEER:
            return "Engineer";
        case CLASS_HEAVY:
            return "Heavy";
        case CLASS_MEDIC:
            return "Medic";
        case CLASS_PYRO:
            return "Pyro";
        case CLASS_SNIPER:
            return "Sniper";
        case CLASS_SOLDIER:
            return "Soldier";
        case CLASS_SPY:
            return "Spy";
        default:
            return "Unknown class";
    }
}

#endif /* ENTITY_H_ */
