#include "entityStorage.h"

#include <algorithm>

using namespace std;

void EntityStorage::init()
{

}

EntityStorage::EntityStorage()
{
    init();
}

EntityStorage::~EntityStorage()
{
    clearStorage();
}

void EntityStorage::clearStorage()
{
    removeAllEntities();
    for (auto &model : d_umap)
        delete model.second.first;
}

template <typename T, typename... Args>
struct is_one_of:
        std::disjunction<std::is_same<std::decay_t<T>, Args>...>
{};

void EntityStorage::addEntity(EntityVariants entityVar)
{
    Entity *entity =
    std::visit([](auto&& arg)
    {
        static_assert(is_one_of<decltype(arg), Entity *, Entity_Water *, std::string>{}, "Non matching type.");

        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, Entity *>)
        {
//            qDebug() << "Regular Entity type with ID " << arg->getID() << ":" << arg->getModel()->fileName;
            return arg;
        }
        else if constexpr (std::is_same_v<T, Entity_Water *>)
        {
//            qDebug() << "Entity_Water type with ID " << arg->getID() << ":" << arg->getModel()->fileName;
            return reinterpret_cast<Entity *>(arg);
        }
        else
        {
            qDebug() << "default with value " << arg;
            return nullptr;
        }
    }, entityVar);

    if (!insertModel(entity->getModel()))
    {
        qDebug() << "Failed to add Entity:" << entity->getID();
        return;
    }
    auto entities = getEntitiesOfModel(entity->getModel());
    if (entities == nullptr)
    {
        qDebug() << "Model returned nullptr! Was trying to find model:" << entity->getModel()->getQFileName();
        delete entity;
        return;
    }
    entities->push_back(entity);
}

InsertStatus EntityStorage::insertModel(Model *model)
{
    if (model == nullptr)
    {
        qDebug() << "Trying to insert nullptr model!";
        return InsertStatus::FAIL;
    }
    auto foundModel = d_umap.find(model->getFileName());
    // If a model doesn't exist yet, simply add it.
    if (foundModel == d_umap.end())
        d_umap.insert(model->getFileName(), ModelPair(model, vector<Entity *>()));
    // If it does already exist, delete the one that was to be inserted and replace the pointer by the model that
    // Already existed.
    else if (foundModel->second.first->getFileName() != model->getFileName())
    {
        delete model;
        return InsertStatus::ALREADYEXISTED;
    }
    return InsertStatus::SUCCESS;
}

vector<Entity *> *EntityStorage::getEntitiesOfModel(Model const *model)
{
    if (d_umap.find(model->getFileName()) == d_umap.end())
        return nullptr;
    return &d_umap.find(model->getFileName())->second.second;
}

vector<Entity *> const *EntityStorage::getEntitiesOfModel(Model const *model) const
{
    if (d_umap.find(model->getFileName()) != d_umap.end())
        return nullptr;
    return &d_umap.find(model->getFileName())->second.second;
}

std::vector<Entity *> *EntityStorage::getEntitiesOfModel(ModelMapEntry &entry)
{
    return &entry.second.second;
}

std::vector<Entity *> const *EntityStorage::getEntitiesOfModel(ModelMapEntry const &entry) const
{
    return &entry.second.second;
}

Model *EntityStorage::getModel(ModelMapEntry const &entry)
{
    return entry.second.first;
}

Model const *EntityStorage::getModel(ModelMapEntry const &entry) const
{
    return entry.second.first;
}

Model *EntityStorage::getModel(string const &modelName)
{
    for (auto &model : d_umap)
        if (model.second.first->getFileName() == modelName)
            return model.second.first;
    return nullptr;
}

Model const *EntityStorage::getModel(string const &modelName) const
{
    for (auto &model : d_umap)
        if (model.second.first->getFileName() == modelName)
            return model.second.first;
    return nullptr;
}

Model *EntityStorage::getModel(string const &&modelName)
{
    for (auto &model : d_umap)
        if (model.second.first->getFileName() == modelName)
            return model.second.first;
    return nullptr;
}

Model const *EntityStorage::getModel(string const &&modelName) const
{
    for (auto &model : d_umap)
        if (model.second.first->getFileName() == modelName)
            return model.second.first;
    return nullptr;
}

Entity *EntityStorage::updateEntity(Entity *entity, Model *model)
{
    // No action needed.
    if (entity->getModel() == model)
        return entity;

    // Remove entity from its current model's entity vector.
    auto entities = getEntitiesOfModel(entity->getModel());
    for (auto it = entities->begin(); it != entities->end(); ++it)
    {
        if ((*it)->getID() == entity->getID())
        {
            entities->erase(it);
            break;
        }
    }

    entity->setModel(model);

    // No entity specialization required, so it can just be added to the new model entity vector.
    if (entity->getShaderType() == model->getShaderType())
    {
        getEntitiesOfModel(model)->push_back(entity);
        return entity;
    }

    // If the entity is currently a water type and moving to a different type, get rid of all non-standard stuff.
    if (entity->getShaderType() == ShaderType::WATER)
    {
        reinterpret_cast<Entity_Water *>(entity)->toRegular();
        // When creating a new entity from an old one WITHOUT generating a new ID, the constructor will delete
        // the old entity. This is because there can be no 2 entities with the same ID.
        Entity *newEntity = new Entity(entity, false);
        newEntity->setModel(model);
        entity = newEntity;
    }

    // If the entity is becoming a water-type entity, generate the water-specific stuff.
    if (model->getShaderType() == ShaderType::WATER)
    {
        Entity_Water *water = new Entity_Water(entity, false);
        water->setModel(model);
        water->generateAABB();
        water->setShaderType(model->getShaderType());
        addEntity(water);
        return reinterpret_cast<Entity *>(water);
    }

    getEntitiesOfModel(model)->push_back(entity);
    entity->setShaderType(model->getShaderType());
    return entity;
}

Entity *EntityStorage::duplicateEntity(Entity *entity)
{
    if (entity->getShaderType() == ShaderType::DEFAULT)
    {
        Entity *newEntity = new Entity(entity);
        addEntity(newEntity);
        return newEntity;
    }
    else if (entity->getShaderType() == ShaderType::WATER)
    {
        Entity_Water *newEntity = new Entity_Water(entity);
        addEntity(newEntity);
        return newEntity;
    }
    return nullptr;
}

void EntityStorage::removeEntity(Entity *remove)
{
    for (auto &model : d_umap)
    {
        vector<Entity *> *entities = getEntitiesOfModel(model.second.first);

        if (entities == nullptr)
            continue;

        for (auto it = entities->begin(); it != entities->end(); ++it)
        {
            if ((*it)->getID() == remove->getID())
            {
                delete *it;
                entities->erase(it);
                return;
            }
        }
    }
}

void EntityStorage::removeAllEntities()
{
    for (auto &model : d_umap)
    {
        vector<Entity *> *entities = getEntitiesOfModel(model.second.first);

        if (entities == nullptr)
            continue;

        for (auto *ent : *entities)
            delete ent;
        entities->clear();
    }
}

ModelMap::const_iterator EntityStorage::begin() const
{
    return d_umap.begin();
}

ModelMap::const_iterator EntityStorage::end() const
{
    return d_umap.end();
}

ModelMap::iterator EntityStorage::begin()
{
    return d_umap.begin();
}

ModelMap::iterator EntityStorage::end()
{
    return d_umap.end();
}

size_t EntityStorage::size() const
{
    return d_umap.size();
}

size_t EntityStorage::entityCount() const
{
    size_t ret = 0;
    for (auto &model : d_umap)
        ret += model.second.second.size();
    return ret;
}

Entity *EntityStorage::getEntityAt(size_t pos)
{
    Entity *ret = nullptr;
    size_t count = 0;
    size_t max;
    for (auto &model : d_umap)
    {
        max = count + model.second.second.size();
        if (pos >= count && pos < max)
        {
            ret = model.second.second.at(pos - count);
            break;
        }
        count += model.second.second.size();
    }
    return ret;
}
