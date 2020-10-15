# boost_optional_ext is a pipe operator for the boost::optional

# Examples

Consider a given function:

```C++
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

CheckStatus checkActionIsStarted(const std::string& action, const TParams& data)
{
    using namespace ipa::util;
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
    return result.value_or(CheckStatus::eNotProvided);
}

void MyClass::Test()
{
  TParams actionResults = {
      {"xyz-1", std::make_tuple(StatusType::eFinished, false)},
      {"xyz-2", std::make_tuple(StatusType::eStarted, true)},
      {"xyz-3", std::make_tuple(StatusType::eInProgress, false)},
      {"xyz-4", std::make_tuple(StatusType::eFinished, true)}
  };

  std::cout << "xyz-1: " << (checkActionIsStarted("xyz-1", actionResults) == CheckStatus::eOk) << std::endl;
  std::cout << "xyz-2: " << (checkActionIsStarted("xyz-2", actionResults) == CheckStatus::eOk) << std::endl;
  std::cout << "xyz-3: " << (checkActionIsStarted("xyz-3", actionResults) == CheckStatus::eOk) << std::endl;
  std::cout << "xyz-4: " << (checkActionIsStarted("xyz-4", actionResults) == CheckStatus::eOk) << std::endl;
}

```
## Output is

```
xyz-1: 0
xyz-2: 1
xyz-3: 0
xyz-4: 0
```

