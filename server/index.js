const express = require('express')
const bodyParser = require('body-parser')

const app = express();
app.use(bodyParser.urlencoded({ extended: false }));

var vm_addon = require('bindings')('napi_wrap_vm');

var vm = new vm_addon.VMWrap('src/challenge.bin');

const loc_re = /== (\w[\w ]+[\w!]) ==/i;
const li_re = /(\w[\w ]+$)/img;


app.get('/vm', (req, res) => {
  const command = req.query.command || '';
  res.setHeader('Content-Type', 'application/json');

  console.log('request with command = ' + command);

  // Use command to choose which VM function to call
  var vm_out;
  if (command == 'reset') {
    vm_out = vm.reset().trim();
  } else if (command == 'revert') {
    const timestep = req.query.step || 0;
    vm_out = vm.revert(parseInt(timestep)).trim();
  } else {
    vm_out = vm.execute(command).trim();
  }

  // Parse VM output and build json response
  var response = {};
  
  if (loc_re.test(vm_out)) {
    const s_ind = vm_out.search(loc_re);

    if (s_ind > 0) {
      response["message"] = vm_out.slice(0, s_ind).trim();
    }
    
    vm_out = vm_out.slice(s_ind);

    const items_ind = vm_out.search(/Things of interest here:/);
    const exits_ind = vm_out.search(/There (?:(?:is)|(?:are)) \d exits?:/);

    response["location"] = vm_out.match(loc_re)[1];
    
    if (items_ind == -1) {
      response["description"] = vm_out.slice(vm_out.search('\n'), exits_ind).trim();
      response["items"] = [];
    } else {
      response["description"] = vm_out.slice(vm_out.search('\n'), items_ind).trim();
      response["items"] = vm_out.slice(items_ind, exits_ind).match(li_re);
    }
    response["exits"] = vm_out.slice(exits_ind).match(li_re);

  } else {
    response["message"] = vm_out.trim();
  }

  res.send(JSON.stringify(response));
});

app.get('/vm/inv', (req, res) => {
  var vm_out = vm.execute('inv').trim();

  console.log("request to check inventory");

  response = { inventory: vm_out.match(li_re) || [] };

  res.send(JSON.stringify(response))
});

app.listen(3001, () =>
  console.log('Express server is running on localhost:3001')
);
