#include "MCIMAPNamespaceItem.h"

using namespace mailcore;

static Array * encodedComponents(Array * components);
static Array * decodedComponents(Array * components);

void IMAPNamespaceItem::init()
{
    mDelimiter = 0;
    mPrefix = NULL;
}

IMAPNamespaceItem::IMAPNamespaceItem()
{
    init();
}

IMAPNamespaceItem::IMAPNamespaceItem(IMAPNamespaceItem * other)
{
    init();
    setDelimiter(other->delimiter());
    setPrefix(other->prefix());
}

IMAPNamespaceItem::~IMAPNamespaceItem()
{
    MC_SAFE_RELEASE(mPrefix);
}

String * IMAPNamespaceItem::description()
{
    return String::stringWithUTF8Format("<%s:%p %s %c>",
        MCUTF8(className()), this, MCUTF8(prefix()), delimiter());
}

Object * IMAPNamespaceItem::copy()
{
    return new IMAPNamespaceItem(this);
}

void IMAPNamespaceItem::setPrefix(String * prefix)
{
    MC_SAFE_REPLACE_COPY(String, mPrefix, prefix);
}

String * IMAPNamespaceItem::prefix()
{
    return mPrefix;
}

void IMAPNamespaceItem::setDelimiter(char delimiter)
{
    mDelimiter = delimiter;
}

char IMAPNamespaceItem::delimiter()
{
    return mDelimiter;
}

String * IMAPNamespaceItem::pathForComponents(Array * components)
{
    String * path;
    String * prefix;
    
    components = encodedComponents(components);
    path = components->componentsJoinedByString(String::stringWithUTF8Format("%c", mDelimiter));
    
    prefix = mPrefix;
    if (prefix->length() > 0) {
        if (!prefix->hasSuffix(String::stringWithUTF8Format("%c", mDelimiter))) {
            prefix = prefix->stringByAppendingUTF8Format("%c", mDelimiter);
        }
    }
    return prefix->stringByAppendingString(path);
}

Array * IMAPNamespaceItem::componentsForPath(String * path)
{
    Array * components;
    
    if (path->hasPrefix(mPrefix)) {
        path = path->substringFromIndex(mPrefix->length());
    }
    components = path->componentsSeparatedByString(String::stringWithUTF8Format("%c", mDelimiter));
    components = decodedComponents(components);
    if (components->count() > 0) {
        if (((String *) components->objectAtIndex(0))->length() == 0) {
            components->removeObjectAtIndex(0);
        }
    }
    
    return components;
}

bool IMAPNamespaceItem::containsFolder(String * folder)
{
    if (mPrefix->length() == 0)
        return true;
    return folder->hasPrefix(mPrefix);
}

void IMAPNamespaceItem::importIMAPNamespaceInfo(struct mailimap_namespace_info * info)
{
    setPrefix(String::stringWithUTF8Characters(info->ns_prefix));
    setDelimiter(info->ns_delimiter);
}

static Array * encodedComponents(Array * components)
{
    Array * result;
    
    result = Array::array();
    for(unsigned int i = 0 ; i < components->count() ; i ++) {
        String * value = (String *) components->objectAtIndex(i);
        result->addObject(value->mUTF7EncodedString());
    }
    
    return result;
}

static Array * decodedComponents(Array * components)
{
    Array * result;
    
    result = Array::array();
    for(unsigned int i = 0 ; i < components->count() ; i ++) {
        String * value = (String *) components->objectAtIndex(i);
        String * decoded;
        
        decoded = value->mUTF7DecodedString();
        if (decoded == NULL) {
            decoded = value;
        }
        result->addObject(decoded);
    }
    
    return result;
}
