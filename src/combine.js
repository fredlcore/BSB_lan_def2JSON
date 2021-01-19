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

var comArray = [];

for (i = 0; i < entries.length; i++) {
  let item = entries[i];

  if (item.toLocaleLowerCase().endsWith('.json') && !item.toLocaleLowerCase().endsWith('all.json')) {
    let lang = JSON.parse(fs.readFileSync(item));
    combined = mergeDeep(combined, lang);

  }
}

console.log("cleanup");

function cleanUpLang(input) {
  let de = null;
  if (input.hasOwnProperty('DE'))
    de = input['DE'];

  let en = null;
  if (input.hasOwnProperty('EN'))
    en = input['EN'];


  if (de == en)
    delete input['EN'];

  for (let key in input) {
    if (key != "DE" && key != "EN") {
      if (de == input[key] || en == input[key])
        delete input[key];
    }
  }
  return input;
}

let categories = {};
for (let key in combined.commands) {
  let item = combined.commands[key];

  if (!item.category)
    continue;

  let categorie = categories[item.category.min];
  if (!categorie) {
    if (item.category && item.category.name)
      item.category.name = cleanUpLang(item.category.name);

    categories[item.category.min] = item.category;
    categorie = item.category;
    categorie.commands = [];
  }
  delete item.category;

  if (item.description)
    item.description = cleanUpLang(item.description);

  if (item.enum) {
    for (let enumKey in item.enum) {
      item.enum[enumKey] = cleanUpLang(item.enum[enumKey]);
    }
  }

  if (item.type && item.type.unit)
    item.type.unit = cleanUpLang(item.type.unit);

  if (item.flags) {
    item.flags = item.flags.sort();

    let idx = item.flags.lastIndexOf('')
    if (idx > -1)
      item.flags = item.flags.slice(idx + 1);

    if (item.flags.length == 0)
      delete item.flags;
  }

  // if (item.device && item.device.length == 1 && item.device[0].family == 255 && item.device[0].family == 255)
  //   delete item.device;

  if (item.dummy !== undefined)
    delete item.dummy;

  let copyitem = { ...item };
  delete copyitem.device;
  let compareItem = JSON.stringify(copyitem);

  for (const cmd of categorie.commands) {
    let copycmd = { ...cmd };
    delete copycmd.device;
    let compareCmd = JSON.stringify(copycmd);

    if (compareCmd == compareItem) {
      cmd.device = cmd.device.concat(item.device)
      item = null;
      break;
    }
  }
  if (item != null)
    categorie.commands.push(item);
}

combined.version = combined.Version;
delete combined.Version;

combined.compiletime = combined.Compiletime;
delete combined.Compiletime;

combined.categories = categories;
delete combined.commands;

// fix as Workaround for ISSUE https://github.com/fredlcore/bsb_lan/issues/250
for (const catKey in combined.categories) {
  const cat = combined.categories[catKey]
  for (const cmd of cat.commands) {

    const type = cmd.type
    if (type.name == "TIMEPROG")
      type.datatype = "TMPR"
    
      type.payload_flags = type.payload_length & 0xE0
      type.payload_length = type.payload_length & 0x1F
      //type.enable_byte_read = 
     // enable_byte
  }
}


 // for regular commands either 1 or 6. 8 indicates data type does not use enable. 0 indicates no set telegram has been logged to determine correct enable byte.


  // length of payload in byte; +32 if special treatment is needed, +64 if payload length can vary; 0 for read-only type or unknown length


// walk through the tree and fix the tree
// idx: object -> array
// remove "" flags
// combine lang if missing

fs.writeFileSync('small_all.json', JSON.stringify(combined));
fs.writeFileSync('all.json', JSON.stringify(combined, null, 1));