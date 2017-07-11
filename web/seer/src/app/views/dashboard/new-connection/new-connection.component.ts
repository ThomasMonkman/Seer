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
      this.connectionForm.get('address').valueChanges.startWith(''),
      this.connectionForm.get('name').valueChanges.startWith('')
    )
    .filter((val: [ConnectionMeta[], string, string]) => val[0] !== null)
    .subscribe(val => this.filterHistory(val));
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
    console.log('selectCheck');
    return this.connectionForm.get('address').value === connection.address && this.connectionForm.get('name').value === connection.name;
  }

  protected removeHistory(connection: ConnectionMeta) {
    console.log('removing ', connection.name);
    this.connectionHistory.remove(connection.name);
  }

  private filterHistory(val: [ConnectionMeta[], string, string]) {
    console.log('filtering')
    const [pastConnections, address, name] = val;
    //need to add sort as well
    this.filteredPastConnections = pastConnections
      // filter by both fields      
      .filter((connection: ConnectionMeta) => {
        //if both fields are empty we should show the whole history
        const emptyFields = (address === '' && name === '');
        if (emptyFields) {
          return true;
        } else {
          //if either one of the fields contain data, we should filter by them both, exlucing
          const addressFilter = (address === '') ? false : connection.address.toLowerCase().includes(address.toLowerCase());
          const nameFilter = (name === '') ? false : connection.name.toLowerCase().includes(name.toLowerCase());

          return addressFilter || nameFilter;
        }
      })
      .map(a => {
        console.log(a);
        return a;
      })
      .sort((a: ConnectionMeta, b: ConnectionMeta) => {
        const addressA = a.address.toLowerCase();
        const addressB = b.address.toLowerCase();
        return +(addressA > addressB) || +(addressA === addressB) - 1;
      })
      .sort((a: ConnectionMeta, b: ConnectionMeta) => {        
        const nameA = a.name.toLowerCase();
        const nameB = b.name.toLowerCase();
        return +(nameA > nameB) || +(nameA === nameB) - 1;
      });
  }
}
