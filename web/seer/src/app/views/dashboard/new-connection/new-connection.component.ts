import { Component, OnInit } from '@angular/core';
import { FormControl, FormGroup, Validators } from '@angular/forms';
import { ConnectionMeta } from 'app/classes/connnection-meta';
import { ConnectionHistoryService } from 'app/services/connection-history/connection-history.service';
import { Observable } from 'rxjs/Observable';

@Component({
  selector: 'app-new-connection',
  templateUrl: './new-connection.component.html',
  styleUrls: ['./new-connection.component.scss']
})
export class NewConnectionComponent {
  protected connectionForm = new FormGroup({
    address: new FormControl('', [Validators.required]),
    name: new FormControl(''),
  });

  protected filteredPastConnections: ConnectionMeta[] = [];// = [new ConnectionMeta('hello', 'hello')];

  constructor(private connectionHistory: ConnectionHistoryService) {
    Observable.combineLatest(
      this.connectionHistory.get(),
      this.connectionForm.get('address').valueChanges.startWith(null),
      this.connectionForm.get('name').valueChanges.startWith(null)
    ).subscribe(val => this.filterHistory(val));
  }

  protected connect() {
    if (this.connectionForm.status === 'VALID') {
      console.log('connect');
    }
  }

  protected historySelected(connection: ConnectionMeta) {
    this.connectionForm.setValue({ address: connection.address, name: connection.name });
  }

  protected historySameAsInput(connection: ConnectionMeta): boolean {
    return this.connectionForm.get('address').value === connection.address && this.connectionForm.get('name').value === connection.name;
  }

  private filterHistory(val: [ConnectionMeta[], string, string]) {
    const [pastConnections, address, name] = val;
    //need to add sort as well
    this.filteredPastConnections = pastConnections.filter(connection => {
      let addressFilter = address ? connection.address.toLowerCase().includes(address.toLowerCase()) : true;
      let nameFilter = name ? connection.name.toLowerCase().includes(name.toLowerCase()) : true;
      return addressFilter || nameFilter;
    });
  }
}
