import React, { Component } from 'react';
import './App.css';
import Graph from 'react-graph-vis'

const graph_options = {
  layout : {
    hierarchical: false
  },
  edges: {
    color: "#000000"
  },
  physics: {
    enabled: false
  }
};

const node_ids = new Map();

function loc_string(state) {
  return state.location + '@' + state.description + '@' + state.exits.toString();
}

class Clickable extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    return (
      <button className={this.props.type} onClick={() => this.props.onClick()}>
        {this.props.label}
      </button>
    );
  }
}

class App extends Component {

  constructor(props) {
    super(props);
    this.state = {
      location: '',
      description: '',
      items: [],
      exits: [],
      message: '',
      inventory: [],
      history: ['<init>'],
      graph: {
        nodes: [],
        edges: []
      },
      nodeCount: 0,
      prevId: 0
    };
  }

  componentDidMount() {
    fetch(`/vm?command=${encodeURIComponent('reset')}`)
      .then(response => response.json())
      .then(state => {
        this.setState({
          ...state,
          graph: {
            nodes: [
              { id: 1, label: state.location }
            ],
            edges: []
          },
          nodeCount: 1,
          prevId: 1
        });
        node_ids.clear();
        node_ids.set(loc_string(state), 1);
        console.log(node_ids);
      });
  }

  takeItem(itemName) {
    this.handleAction('take ' + itemName);
    const items = this.state.items.slice();
    items.splice(items.indexOf(itemName), 1);
    const inv = this.state.inventory.slice();
    inv.push(itemName);
    this.setState({ items: items, inventory: inv });
  }

  useItem(itemName) {
    if (itemName == 'strange book' || itemName == 'business card' || itemName == 'journal') {
      this.handleAction('look ' + itemName);
    } else {
      this.handleAction('use ' + itemName);
    }
  }

  revert(timestep) {
    console.log("revert to timestep " + timestep);
    fetch(`/vm?command=revert&step=${encodeURIComponent(timestep)}`)
      .then(response => response.json())
      .then(result => {
        this.setState((state, props) => ({
          ...result,
          history: state.history.slice(0, timestep+1)
        }));

        fetch(`/vm?command=look`)
          .then(response => response.json())
          .then(s1 => {
            this.setState(s1);

            fetch('/vm/inv')
              .then(response => response.json())
              .then(s2 => this.setState(s2));
          });
      });
  }

  handleAction(action) {
    fetch(`/vm?command=${encodeURIComponent(action)}`)
      .then(response => response.json())
      .then(result => {
        if (result.location) {
          let next_id;
          let node_count;
          if (node_ids.has(loc_string(result))) {
            next_id = node_ids.get(loc_string(result));
            node_count = this.state.nodeCount;
          } else {
            next_id = this.state.nodeCount+1;
            node_count = this.state.nodeCount+1;
            node_ids.set(loc_string(result), next_id);
          }
          this.setState((state, props) => ({
            ...result,
            history: [...state.history, action],
            graph: {
              nodes: [
                ...state.graph.nodes,
                { id: next_id, label: result.location }
              ],
              edges: [
                ...state.graph.edges,
                {from: state.prevId, to: next_id, label: action}
              ]
            },
            nodeCount: node_count,
            prevId: next_id
          }));
        } else {
          this.setState(result);
          this.setState((state, props) => ({ history: [...state.history, action] }));
          fetch('/vm/inv')
            .then(response => response.json())
            .then(state => this.setState(state));
        }
      });
  }

  render() {

    const items = this.state.items.map((item) => {
      return (
        <li key={item}>
          <Clickable type="item" label={item} onClick={() => this.takeItem(item)}/>
        </li>
      );
    });

    const exits = this.state.exits.map((exit) => {
      return (
        <li key={exit}>
          <Clickable type="exit" label={exit} onClick={() => this.handleAction(exit)}/>
        </li>
      );
    });

    const invs = this.state.inventory.map((item) => {
      return (
        <li key={item}>
          <Clickable type="item" label={item} onClick={() => this.useItem(item)}/>
        </li>
      );
    });

    const history = this.state.history.map((step, idx) => {
      return (
        <li key={idx}>
          <Clickable type="hitem" label={step} onClick={() => this.revert(idx)}/>
        </li>
      );
    });

    return (
      <div className="App">
        <div className="flex-container row" style={{ height: window.innerHeight * 0.5 }}>
          <div className="flex-container col" style={{flexGrow: 4, maxWidth: 1000}}>
            <div className="location" style={{flexGrow: 1}}>
              <h1>{this.state.location}</h1>
              <p style={{fontSize: 18}}>{this.state.description}</p>
            </div>
            <div className="items" style={{flexGrow: 1}}>
              <h2>Available Items</h2>
              <ul>
                {items}
              </ul>
            </div>
            <div className="exits" style={{flexGrow: 1}}>
              <h2>Exits</h2>
              <ul>
                {exits}
              </ul>
            </div>
            <div className="console" style={{flexGrow: 1}}>
              <h2>info</h2>
              <p style={{fontSize: 16}}>{this.state.message}</p>
            </div>
          </div>
          <div className="flex-container col" style={{flexGrow: 1, width:200}}> 
            <div className="inventory" style={{flexGrow: 1}}>
                <h2>Inventory</h2>
                <p>click to use an item!</p>
                <ul>
                  {invs}
                </ul>
            </div>
          </div>
          <div className="flex-container col" style={{flexGrow: 1, width:250}}>
            <div className="history" style={{flexGrow: 1}}>
              <h2>History</h2>
              <p>click to go back in time!</p>
              <ol type="1">
                {history}
              </ol>
            </div>
          </div>
        </div>
        <Graph graph={this.state.graph} options={graph_options} style={{height: window.innerHeight * 0.45}}/>
      </div>
    );
  }
}

export default App;