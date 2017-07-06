import { Component, OnInit } from '@angular/core';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { ConnectionHistoryService } from 'app/services/connection-history/connection-history.service';

@Component({
  selector: 'app-new-connection',
  templateUrl: './new-connection.component.html',
  styleUrls: ['./new-connection.component.scss']
})
export class NewConnectionComponent implements OnInit {
  protected connectionForm = new FormGroup({
    address: new FormControl('', [Validators.required]),
    name: new FormControl(''),
  });
  protected filteredStates: any;

  protected pastConnections: ConnectionMeta[];
  states = [
    'Alabama',
    'Alaska',
    'Arizona',
    'Arkansas',
    'Illinois',
    'Indiana',
    'Iowa',
    'Kansas',
    'Kentucky',
    'Louisiana',
    'Maine',
  ];

  constructor(private connectionHistory: ConnectionHistoryService) {
    connectionHistory.store(new ConnectionMeta('localhost', 'localhost'));
    connectionHistory.get().subscribe((connections) => {
      this.pastConnections = connections;
    });

    // this.stateCtrl = new FormControl();
    // this.filteredStates = this.stateCtrl.valueChanges
    //   .startWith(null)
    //   .map(name => this.filterStates(name));
  }

  public ngOnInit() {
  }

  protected connect() {
    if (this.connectionForm.status === 'VALID') {
      console.log('connect');
    }
  }

  private filterStates(val: string) {
    return val ? this.states.filter(s => s.toLowerCase().indexOf(val.toLowerCase()) === 0)
      : this.states;
  }
}
