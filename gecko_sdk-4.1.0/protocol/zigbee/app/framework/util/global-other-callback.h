// This file is generated by Simplicity Studio.  Please do not edit manually.
//
// This file is generated from java code via the following methods:
// com.ember.workbench.x3.core.zclplugin.ZclFrameworkExtension.createCallbackFile()
// com.ember.workbench.x3.core.zclplugin.ZclFrameworkExtension.loadStaticSchema()
// com.ember.workbench.x3.core.zclplugin.ZclFrameworkExtension.doLoadZclAppData()
// com.ember.workbench.x3.core.zclplugin.ZclFrameworkExtension.validate()
// com.ember.workbench.app_framework.application.Framework.validateExtensions()
// com.ember.workbench.app_framework.application.Framework.validateFramework()
// com.ember.workbench.app_framework.ui.ApplicationPreferencePage.accept()
//

#include "af.h"
/** @brief Groups Cluster Clear Group Table
 *
 * This function is called by the framework when the application should clear
 * the group table.
 *
 * @param endpoint The endpoint.  Ver.: always
 */
void emberAfGroupsClusterClearGroupTableCallback(int8u endpoint);
/** @brief Groups Cluster Endpoint In Group
 *
 * This function is called by the framework when it needs to determine if an
 * endpoint is a member of a group.  The application should return true if the
 * endpoint is a member of the group and false otherwise.
 *
 * @param endpoint The endpoint.  Ver.: always
 * @param groupId The group identifier.  Ver.: always
 */
bool emberAfGroupsClusterEndpointInGroupCallback(int8u endpoint,
                                                 int16u groupId);
/** @brief Scenes Cluster ClearSceneTable
 *
 * This function is called by the framework when the application should clear
 * the scene table.
 *
 * @param endpoint The endpoint.  Ver.: always
 */
void emberAfScenesClusterClearSceneTableCallback(int8u endpoint);
/** @brief Scenes Cluster Make Invalid
 *
 * This function is called to invalidate the valid attribute in the Scenes
 * cluster.
 *
 * @param endpoint   Ver.: always
 */
EmberAfStatus emberAfScenesClusterMakeInvalidCallback(int8u endpoint);
/** @brief Scenes Cluster Recall Saved Scene
 *
 * This function is called by the framework when the application should recall a
 * saved scene.
 *
 * @param endpoint The endpoint.  Ver.: always
 * @param groupId The group identifier.  Ver.: always
 * @param sceneId The scene identifier.  Ver.: always
 */
EmberAfStatus emberAfScenesClusterRecallSavedSceneCallback(int8u endpoint,
                                                           int16u groupId,
                                                           int8u sceneId);
/** @brief Scenes Cluster Remove Scenes In Group
 *
 * This function removes the scenes from a specified group.
 *
 * @param endpoint Endpoint  Ver.: always
 * @param groupId Group ID  Ver.: always
 */
void emberAfScenesClusterRemoveScenesInGroupCallback(int8u endpoint,
                                                     int16u groupId);
/** @brief Scenes Cluster Store Current Scene
 *
 * This function is called by the framework when the application should store
 * the current scene.  If an entry already exists in the scene table with the
 * same scene and group ids, the application should update the entry with the
 * current scene.  Otherwise, a new entry should be adde to the scene table, if
 * possible.
 *
 * @param endpoint The endpoint.  Ver.: always
 * @param groupId The group identifier.  Ver.: always
 * @param sceneId The scene identifier.  Ver.: always
 */
EmberAfStatus emberAfScenesClusterStoreCurrentSceneCallback(int8u endpoint,
                                                            int16u groupId,
                                                            int8u sceneId);
/** @brief On/off Cluster Level Control Effect
 *
 * This is called by the framework when the on/off cluster initiates a command
 * that must effect a level control change. The implementation assumes that the
 * client will handle any effect on the On/Off Cluster.
 *
 * @param endpoint   Ver.: always
 * @param newValue   Ver.: always
 */
void emberAfOnOffClusterLevelControlEffectCallback(int8u endpoint,
                                                   bool newValue);
/** @brief On/off Cluster Set Value
 *
 * This function is called when the on/off value needs to be set, either through
 * normal channels or as a result of a level change.
 *
 * @param endpoint   Ver.: always
 * @param command   Ver.: always
 * @param initiatedByLevelChange   Ver.: always
 */
EmberAfStatus emberAfOnOffClusterSetValueCallback(int8u endpoint,
                                                  int8u command,
                                                  bool initiatedByLevelChange);
/** @brief Key Establishment Cluster Client Command Received
 *
 * This function is called by the application framework when a server-to-client
 * key establishment command is received but has yet to be handled by the
 * framework code.  This function should return a bool value indicating whether
 * the command has been handled by the application code and should not be
 * further processed by the framework.
 *
 * @param cmd   Ver.: always
 */
bool emberAfKeyEstablishmentClusterClientCommandReceivedCallback(EmberAfClusterCommand *cmd);
/** @brief Key Establishment Cluster Server Command Received
 *
 * This function is called by the application framework when a client-to-server
 * key establishment command is received but has yet to be handled by the
 * framework code.  This function should return a bool value indicating whether
 * the command has been handled by the application code and should not be
 * further processed by the framework.
 *
 * @param cmd   Ver.: always
 */
bool emberAfKeyEstablishmentClusterServerCommandReceivedCallback(EmberAfClusterCommand *cmd);
