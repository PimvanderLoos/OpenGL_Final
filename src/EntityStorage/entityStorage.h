#ifndef GAME_ENTITYSTORAGE_H
#define GAME_ENTITYSTORAGE_H

#include <shared_mutex>
#include <unordered_map>
#include <vector>
#include <iterator>
#include <variant>
#include <cassert>

#include "../Entity/entity.h"
#include "../Entity/entity_water.h"
#include "../ThreadSafeUnorderedMap/threadSafeUnorderedMap.h"

using EntityVariants = std::variant<Entity *, Entity_Water *>;
using ModelPair = std::pair<Model *, std::vector<Entity *>>;
using ModelMapEntry = std::pair<std::string const, ModelPair>;
using ModelMap = ThreadSafeUnorderedMap<std::string, ModelPair>;

enum InsertStatus
{
    FAIL, SUCCESS, ALREADYEXISTED
};

class EntityStorage
{
    private:
        ModelMap d_umap;
        void init();

    public:
        EntityStorage();
        ~EntityStorage();

        void clearStorage();

        void addEntity(EntityVariants entity);
        InsertStatus insertModel(Model *model);

        std::vector<Entity *> *getEntitiesOfModel(Model const *model);
        std::vector<Entity *> const *getEntitiesOfModel(Model const *model) const;

        std::vector<Entity *> *getEntitiesOfModel(ModelMapEntry &entry);
        std::vector<Entity *> const *getEntitiesOfModel(ModelMapEntry const &entry) const;

        Model *getModel(std::string const &modelName);
        Model const *getModel(std::string const &modelName) const;

        Model *getModel(ModelMapEntry const &entry);
        Model const *getModel(ModelMapEntry const &entry) const;

        Model *getModel(std::string const &&modelName);
        Model const *getModel(std::string const &&modelName) const;

        Entity *updateEntity(Entity *entity, Model *model);
        Entity *duplicateEntity(Entity *entity);

        void removeEntity(Entity *remove);
        void removeAllEntities();

        ModelMap::const_iterator begin() const;
        ModelMap::const_iterator end() const;
        ModelMap::iterator begin();
        ModelMap::iterator end();

        size_t size() const;

        size_t entityCount() const; // Get total number of entities for all models.
        Entity *getEntityAt(size_t pos); // Get entity at position pos (between 0 and entityCount).
};

#endif
