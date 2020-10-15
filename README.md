# boost_optional_ext is a pipe operator for the boost::optional

# Examples

Consider a given function:

```C++
namespace helper {

template <typename TMapContainer, typename TKey = typename TMapContainer::key_type, typename TValue = typename TMapContainer::mapped_type>
boost::optional<const TValue&> findInMap(const TMapContainer& container, const TKey& key)
{
    auto foundIt = container.find(key);
    if (foundIt != container.cend())
    {
        return boost::optional<const TValue&>(foundIt->second);
    }
    else
    {
        return boost::none;
    }
}

} // end namespace helper
```

and a code snippet:

```C++
enum StatusType {
    eStarted = 0,
    eInProgress,
    eFinished,
    eStatusTypeEnd
};

enum CheckStatus {
    eOk = 0,
    eNotOk,
    eNotProvided,
    eCheckStatusEnd
};

//----------------------|action----|-------|status_type-|-isOk-|
using TParams = std::map<std::string, std::tuple<StatusType, bool>>;

CheckStatus isActionStarted(const std::string& action, const TParams& data)
{
    auto result = helper::find(data, action)
        | [](auto&& el) -> boost::optional<const TParams::mapped_type&> {
            boost::optional<const TParams::mapped_type&> ret = boost::none;
            if (std::get<0>(el) == StatusType::eStarted)
            {
                ret = el;
            }
            return ret;
        }
        | [](auto&& el) {
            return std::get<1>(el) ? CheckStatus::eOk : CheckStatus::eNotOk;
        };
    return result <<= CheckStatus::eNotProvided;
}

void MyClass::Test()
{
  TParams actionsResults = {
      {"xyz-1", std::make_tuple(StatusType::eFinished, false)},
      {"xyz-2", std::make_tuple(StatusType::eStarted, true)},
      {"xyz-3", std::make_tuple(StatusType::eInProgress, false)},
      {"xyz-4", std::make_tuple(StatusType::eFinished, true)}
  };

  std::cout << "xyz-1: " << (isActionStarted("xyz-1", actionsResults) == CheckStatus::eOk) << std::endl;
  std::cout << "xyz-2: " << (isActionStarted("xyz-2", actionsResults) == CheckStatus::eOk) << std::endl;
  std::cout << "xyz-3: " << (isActionStarted("xyz-3", actionsResults) == CheckStatus::eOk) << std::endl;
  std::cout << "xyz-4: " << (isActionStarted("xyz-4", actionsResults) == CheckStatus::eOk) << std::endl;
}

```
## Output is

```
xyz-1: 0
xyz-2: 1
xyz-3: 0
xyz-4: 0
```

