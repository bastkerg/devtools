/*
 * Copyright (c) 2020-2023 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PROJMGREXTGENERATOR_H
#define PROJMGREXTGENERATOR_H

#include "ProjMgrParser.h"
#include "ProjMgrUtils.h"

 /**
  * @brief external generator item containing
  *        component identifier
  *        directory for generated files
  *        project type
 */
struct ExtGeneratorItem {
  std::string componentId;
  std::string genDir;
  std::string projectType;
};

/**
 * @brief map of used generators, directories and contexts
*/
typedef std::map<std::string, StrVecMap> GeneratorContextVecMap;

/**
 * @brief solution/project types
*/
static constexpr const char* TYPE_SINGLE_CORE = "single-core";
static constexpr const char* TYPE_MULTI_CORE = "multi-core";
static constexpr const char* TYPE_TRUSTZONE = "trustzone";

/**
 * @brief projmgr external generator class responsible for handling global generators
*/
class ProjMgrExtGenerator {
public:
  /**
   * @brief class constructor
  */
  ProjMgrExtGenerator(ProjMgrParser* parser);


  /**
   * @brief class destructor
  */
  ~ProjMgrExtGenerator(void);

  /**
   * @brief set check schema
   * @param boolean check schema
  */
  void SetCheckSchema(bool checkSchema);

  /**
   * @brief retrieve globally registered generators
   * @return true if successful
  */
  bool RetrieveGlobalGenerators(void);

  /**
   * @brief verify if generator is global
   * @param generatorId generator identifier
   * @return true if generator is global
  */
  bool IsGlobalGenerator(const std::string& generatorId);

  /**
   * @brief verify if generator required by a given component is valid
   * @param generatorId generator identifier
   * @param componentId component identifier
   * @return true if generator is valid
  */
  bool CheckGeneratorId(const std::string& generatorId, const std::string& componentId);

  /**
   * @brief get directory for generated files
   * @param generatorId generator identifier
   * @return string directory for generated files
  */
  const std::string& GetGlobalGenDir(const std::string& generatorId);

  /**
   * @brief get run command for generator call
   * @param generatorId generator identifier
   * @return string with run command for generator call
  */
  const std::string& GetGlobalGenRunCmd(const std::string& generatorId);

  /**
   * @brief add generator to the list of used generators of a given context
   * @param generatorId generator identifier
   * @param genDir directory for generated files
   * @param contextId context identifier
  */
  void AddUsedGenerator(const std::string& generatorId, const std::string& genDir, const std::string& contextId);

  /**
   * @brief get map of used generators
   * @return map of used generators
  */
  const GeneratorContextVecMap& GetUsedGenerators(void);

  /**
   * @brief get layer item with generator-import file data
   * @param contextId context identifier
   * @param boolean reference, true if successful
   * @return layer item
  */
  ClayerItem* GetGeneratorImport(const std::string& contextId, bool& success);

protected:
  ProjMgrParser* m_parser = nullptr;
  StrVec m_globalGeneratorFiles;
  std::map<std::string, GlobalGeneratorItem> m_globalGenerators;
  GeneratorContextVecMap m_usedGenerators;
  bool m_checkSchema;
  std::string m_compilerRoot;
};

#endif  // PROJMGREXTGENERATOR_H
