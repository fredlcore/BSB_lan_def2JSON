const fs = require("fs");

/**
* Performs a deep merge of objects and returns new object. Does not modify
* objects (immutable) and merges arrays via concatenation.
*
* @param {...object} objects - Objects to merge
* @returns {object} New object with merged key/values
*/
function mergeDeep(...objects) {
  const isObject = obj => obj && typeof obj === 'object';

  return objects.reduce((prev, obj) => {
    Object.keys(obj).forEach(key => {
      const pVal = prev[key];
      const oVal = obj[key];

      if (Array.isArray(pVal) && Array.isArray(oVal)) {
        prev[key] = pVal.concat(...oVal);
      }
      else if (isObject(pVal) && isObject(oVal)) {
        prev[key] = mergeDeep(pVal, oVal);
      }
      else {
        prev[key] = oVal;
      }
    });

    return prev;
  }, {});
}

let combined = {};

var entries = fs.readdirSync('./');

for (i = 0; i < entries.length; i++) {
  let item = entries[i];

  if (item.toLocaleLowerCase().endsWith('.json')) {
    let lang = JSON.parse(fs.readFileSync(item));

    combined = mergeDeep(combined, lang);
  }
}

// walk through the tree and fix the tree
// idx: object -> array
// remove "" flags
// combine lang if missing

fs.writeFileSync('all.json', JSON.stringify(combined, null, 1));